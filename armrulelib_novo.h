#ifndef ARMRULELIB1_H
#define ARMRULELIB1_H
#include <sstream>
#include <math.h>
#include <string>
#include <cstdio>
#include <stdlib.h>

#define BUFFSIZE 128

#define L1 7.3
#define L2 14.8
#define L3 18.5
#define L4 8.7

extern "C"{
	#include "ufrn_al5d.h"
}

int serial_retorno;
char *comando;

void posicao_inicial();

void init() {
	ufrn_header();
	serial_retorno = abrir_porta();
	if (configurar_porta(serial_retorno) == -1){
		printf("Erro na abertura da porta\n");
		close(serial_retorno);
	}
	comando = (char*) malloc(sizeof(char)*BUFFSIZE);
}


int calcular_posicao_base(float teta) {
	int pos = (int) (0.00623 * pow(teta, 2) - 9.95 * teta + 1345);
	return pos;
}

int calcular_posicao_ombro(float teta) {
	int pos = (int) (0.00444 * pow(teta, 2) + 8.35555 * teta + 649);
	return pos;
}

int calcular_posicao_cotovelo(float teta) {
	int pos = (int) (0.01497 * pow(teta, 2) - 7.65185 * teta + 803);
	return pos;
}

int calcular_posicao_punho(float teta) {
	int pos = (int) (-0.00277 * pow(teta, 2) + 10.15 * teta + 1503);
	return pos;
}

double degreesToRadians(double graus) {
	return (graus * M_PI)/180.0;
}

double radiansToDegrees(double rad) {
	return ((rad * 180.0)/M_PI);
}

void calcularCinematicaInversa(double posX, double posY, double posZ, double orientacao) {

	orientacao = degreesToRadians(orientacao);
	double lTotalXY = sqrt(pow(posY,2) + pow(posX,2));
	double sinThetaBase = posY/lTotalXY;
	double cosThetaBase = posX/lTotalXY;
	double thetaBase = atan2(sinThetaBase, cosThetaBase);

	double x4 =  (cos(orientacao))*L4;
	double z4 =  (sin(orientacao))*L4;

	double x41 = lTotalXY - x4;
	double z41 = posZ - L1 - z4;

	double cosThetaCotovelo = (pow(x41, 2) + pow(z41, 2) - pow(L2, 2) - pow(L3,2))/(2*L2*L3);
	double sinThetaCotovelo = (-1)*sqrt(1 - pow(cosThetaCotovelo,2));

	double thetaCotovelo = atan2(sinThetaCotovelo, cosThetaCotovelo);

	double lXZ41 = sqrt(pow(x41, 2)+ pow(z41,2));

	double sinAlpha = (posZ - L1 - z4)/lXZ41;
	double cosAlpha = (lTotalXY - x4)/lXZ41;

	double sinBeta = (L3*sin(thetaCotovelo))/lXZ41;
	double cosBeta = (L2 + L3*cos(thetaCotovelo))/lXZ41;

	double alpha = atan2(sinAlpha, cosAlpha);
	double beta = atan2(sinBeta, cosBeta);

	double thetaOmbro = alpha - beta;


	double thetaPunho = orientacao - thetaOmbro - thetaCotovelo;

	memset(comando, 0, BUFFSIZE);
	thetaBase = radiansToDegrees(thetaBase);
	thetaOmbro = radiansToDegrees(thetaOmbro);
	thetaCotovelo = radiansToDegrees(thetaCotovelo);
	thetaPunho = radiansToDegrees(thetaPunho);
  	sprintf(comando, "#0P%dS100#1P%dS100#2P%dS100#3P%dS100", calcular_posicao_base(thetaBase), calcular_posicao_ombro(thetaOmbro), calcular_posicao_cotovelo(thetaCotovelo), calcular_posicao_punho(thetaPunho)); 
  	// printf("%s\n", comando);
	enviar_comando(comando, serial_retorno);
}

void pegar() {
	memset(comando, 0, BUFFSIZE);
	sprintf(comando, "#4P2000S100T5000");
	enviar_comando(comando, serial_retorno);
}
void soltar() {
	memset(comando, 0, BUFFSIZE);
	sprintf(comando, "#4P1500S100T5000");
	enviar_comando(comando, serial_retorno);
}
void mover(double x, double y, double z, double phi){
	calcularCinematicaInversa(x, y, z, phi);
}

void posicao_inicial() {
	memset(comando, 0, BUFFSIZE);
	sprintf(comando, "#0P1345S100#1P1436S100#2P803S100#3P1503S100#4P2500S100");
	enviar_comando(comando, serial_retorno);
}

void repouso() {
	memset(comando, 0, BUFFSIZE);
	sprintf(comando, "#0P0000S500#1P0000S500#2P0000S500#3P0000S500#4P0000S500");
	enviar_comando(comando, serial_retorno);
}
#endif
