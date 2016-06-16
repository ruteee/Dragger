
#define DEBUG_LISTS 0
#define DEBUG_LIST_LENGTHS_ONLY 0

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <iostream>
#include "stlastar.h" // See header for copyright and usage information
#include <math.h>


using namespace cv;
using namespace std;

static void help()
{
    cout << "\nThis program demonstrates circle finding with the Hough transform.\n"
            "Usage:\n"
            "./houghcircles <image_name>, Default is pic1.png\n" << endl;
}

const int MAP_WIDTH = 640;
const int MAP_HEIGHT = 480;
int world_map[640*480];
Mat branca(Size(640,480), CV_8UC3, Scalar(255,255, 255));
void a_star(int partida_x, int partida_y, int chegada_x, int chegada_y);
//int world_map[ MAP_WIDTH * MAP_HEIGHT ] =
//{

//};

// map helper functions

int GetMap( int x, int y)
{
	if( x < 0 ||
	    x >= MAP_WIDTH ||
		 y < 0 ||
		 y >= MAP_HEIGHT
	  )
	{
		return 9;
	}

	return world_map[(y*MAP_WIDTH)+x];
}



// Definitions

class MapSearchNode
{
public:
	int x;	 // the (x,y) positions of the node
	int y;

	MapSearchNode() { x = y = 0; }
	MapSearchNode( int px, int py ) { x=px; y=py; }

	float GoalDistanceEstimate( MapSearchNode &nodeGoal );
	bool IsGoal( MapSearchNode &nodeGoal );
	bool GetSuccessors( AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node );
	float GetCost( MapSearchNode &successor );
	bool IsSameState( MapSearchNode &rhs );

	void PrintNodeInfo();
};

bool MapSearchNode::IsSameState( MapSearchNode &rhs )
{

	// same state in a maze search is simply when (x,y) are the same
	if( (x == rhs.x) &&
		(y == rhs.y) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

void MapSearchNode::PrintNodeInfo()
{
	char str[100];
	sprintf( str, "Node position : (%d,%d)\n", x,y );

	cout << str;
}

// Here's the heuristic function that estimates the distance from a Node
// to the Goal.

float MapSearchNode::GoalDistanceEstimate( MapSearchNode &nodeGoal )
{
	return fabsf(x - nodeGoal.x) + fabsf(y - nodeGoal.y);
}

bool MapSearchNode::IsGoal( MapSearchNode &nodeGoal )
{

	if( (x == nodeGoal.x) &&
		(y == nodeGoal.y) )
	{
		return true;
	}

	return false;
}

// This generates the successors to the given Node. It uses a helper function called
// AddSuccessor to give the successors to the AStar class. The A* specific initialisation
// is done for each node internally, so here you just set the state information that
// is specific to the application
bool MapSearchNode::GetSuccessors( AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node )
{

	int parent_x = -1;
	int parent_y = -1;

	if( parent_node )
	{
		parent_x = parent_node->x;
		parent_y = parent_node->y;
	}


	MapSearchNode NewNode;

	// push each possible move except allowing the search to go backwards

	if( (GetMap( x-1, y ) < 9)
		&& !((parent_x == x-1) && (parent_y == y))
	  )
	{
		NewNode = MapSearchNode( x-1, y );
		astarsearch->AddSuccessor( NewNode );
	}

	if( (GetMap( x, y-1 ) < 9)
		&& !((parent_x == x) && (parent_y == y-1))
	  )
	{
		NewNode = MapSearchNode( x, y-1 );
		astarsearch->AddSuccessor( NewNode );
	}

	if( (GetMap( x+1, y ) < 9)
		&& !((parent_x == x+1) && (parent_y == y))
	  )
	{
		NewNode = MapSearchNode( x+1, y );
		astarsearch->AddSuccessor( NewNode );
	}


	if( (GetMap( x, y+1 ) < 9)
		&& !((parent_x == x) && (parent_y == y+1))
		)
	{
		NewNode = MapSearchNode( x, y+1 );
		astarsearch->AddSuccessor( NewNode );
	}

	return true;
}

// given this node, what does it cost to move to successor. In the case
// of our map the answer is the map terrain value at this node since that is
// conceptually where we're moving

float MapSearchNode::GetCost( MapSearchNode &successor )
{
	return (float) GetMap( x, y );

}

int main(int argc, char** argv)
{
    Mat hsv_img;
    Mat yellow;
    Mat img;
    Mat cimg;
    int key = 0;


    //CvCapture* capture = 0;
    //capture = cvCaptureFromCAM(CV_CAP_ANY); //0=default, -1=any camera, 1..99=your camera
    //if (!capture) {
      //cout << "No camera detected" << endl;
    //}
    const char* filename = argc >= 2 ? argv[1] : "board.jpg";
    do {
        img = imread(filename, 1);
    //    img = cvQueryFrame(capture);
        Mat img_new;
        Mat img_trabalho;
        Mat img_configuracao;
        img.copyTo(img_configuracao);

        GaussianBlur(img, img_new,  Size(9, 9), 0.5, 0.5);
    	cvtColor(img_new, hsv_img, COLOR_BGR2HSV);

        //inRange(hsv_img, Scalar(21, 89, 232),Scalar(21, 194 , 255), yellow);



        Mat yellow;
        inRange(hsv_img, Scalar(21, 100, 200),Scalar(30, 200, 255), yellow);
        GaussianBlur(yellow, yellow, Size(9, 9), 4, 4);

        Mat blue;
        inRange(hsv_img, Scalar(100, 60, 100), Scalar(120,210,210), blue);
        GaussianBlur(blue,blue,  Size(9, 9), 4, 4);

        cvtColor(img, cimg, COLOR_BGR2GRAY);
        vector<Vec3f> circles(1);
        GaussianBlur(cimg,cimg,  Size(9, 9), 0.5, 0.5);
        HoughCircles(cimg, circles, CV_HOUGH_GRADIENT, 1, 8,
                     100, 30, 1, 25 // change the last two parameters
                                    // (min_radius & max_radius) to detect larger circles
                     );

        for( size_t i = 0; i < circles.size(); i++ ){
            Vec3i c = circles[i];
            circle(branca, Point(c[0], c[1]), 20, Scalar(0,0,255),-1, 3, CV_AA);
        }

        int partida_x, partida_y, chegada_x, chegada_y;
        //Partida
        HoughCircles(yellow, circles, CV_HOUGH_GRADIENT, 1, 10,
                     100, 30, 20, 38 // change the last two parameters
                                    // (min_radius & max_radius) to detect larger circles
                     );
        for( size_t i = 0; i < circles.size(); i++ ){
            Vec3i c = circles[i];
            circle(branca, Point(c[0], c[1]), 4, Scalar(0,255,255),-1, 3, CV_AA);
            partida_x = c[0];
            partida_y = c[1];
        }

        //Chegada
        HoughCircles(blue, circles, CV_HOUGH_GRADIENT, 1, 10,
                     100, 30, 20, 38 // change the last two parameters
                                    // (min_radius & max_radius) to detect larger circles
                     );

         for( size_t i = 0; i < circles.size(); i++ ){
             Vec3i c = circles[i];
             circle(branca, Point(c[0], c[1]), 4, Scalar(255,0,0),-1, 3, CV_AA);
             chegada_y =c[1];
             chegada_x= c[0];
         }

        //mapeamento de osbtaculos

        bool partida = false;
        bool chegada = false;

        for(int i = 0; i < branca.rows; i++){
            for(int j = 0; j < branca.cols; j++){
                Vec3b bgrPixel = branca.at<Vec3b>(i, j);
                if(bgrPixel[0] == 0 && bgrPixel[1] == 0 && bgrPixel[2]==255){
                    world_map[i*branca.cols + j] = 9;
                }else if(bgrPixel[0] == 255 && bgrPixel[1] == 0 && bgrPixel[2]==0 && !chegada){
                    world_map[i*branca.cols + j] =1 ;
                    chegada = true;
                }else if(bgrPixel[0] == 0 && bgrPixel[1] == 255 && bgrPixel[2]==255 && !partida){
                    world_map[i*branca.cols + j] =1 ;
                    partida = true;
                }else{
                    world_map[i*branca.cols + j] = 1;
                }
                // printf("%d ", world_map[i*branca.cols + j]);
            }
            // printf("\n");
        }

        a_star(partida_x, partida_y, chegada_x, chegada_y);
        imshow("branco", branca);
        waitKey(0);
        key = waitKey(100);
        printf("Tecla: %d\n", key);

  } while (key != 107 || key == -1);
  return 0;
}

void a_star(int partida_x, int partida_y, int chegada_x, int chegada_y){

    	AStarSearch<MapSearchNode> astarsearch;

    	unsigned int SearchCount = 0;

    	const unsigned int NumSearches = 2;

    	while(SearchCount < NumSearches)
    	{

    		// Create a start state
    		MapSearchNode nodeStart;
    		nodeStart.x = partida_x;
    		nodeStart.y = partida_y;

    		// Define the goal state
    		MapSearchNode nodeEnd;
    		nodeEnd.x = chegada_x;
    		nodeEnd.y = chegada_y;

    		// Set Start and goal states

    		astarsearch.SetStartAndGoalStates( nodeStart, nodeEnd );

    		unsigned int SearchState;
    		unsigned int SearchSteps = 0;

    		do
    		{
    			SearchState = astarsearch.SearchStep();
    			SearchSteps++;
    		}
    		while( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SEARCHING );

    		if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SUCCEEDED )
    		{
    			cout << "Search found goal state\n";

    				MapSearchNode *node = astarsearch.GetSolutionStart();

    	#if DISPLAY_SOLUTION
    				cout << "Displaying solution\n";
    	#endif
    				int steps = 0;

    				node->PrintNodeInfo();
    				for( ;; )
    				{
    					node = astarsearch.GetSolutionNext();
    					if( !node )
    					{
    						break;
    					}

                        circle(branca, Point(node->x, node->y), 1, Scalar(0,0,0),-1, 3, CV_AA);
    					node->PrintNodeInfo();
    					steps ++;

    				};

    				cout << "Solution steps " << steps << endl;

    				// Once you're done with the solution you can free the nodes up
    				astarsearch.FreeSolutionNodes();


    		}
    		else if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_FAILED )
    		{
    			cout << "Search terminated. Did not find goal state\n";

    		}

    		// Display the number of loops the search went through
    		cout << "SearchSteps : " << SearchSteps << "\n";

    		SearchCount ++;

    		astarsearch.EnsureMemoryFreed();
    	}
}
