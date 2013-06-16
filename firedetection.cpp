
// FireDetect.cpp : Defines the entry point for the console application.
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <vector>//for vector
#include <math.h>// for sqrt
#include <algorithm>//vector calculation
#include<stdio.h>
#include <fstream>
using namespace std;
int exp_sum = 0;
	int exp_count = 0;

	IplImage* frame[30];
	IplImage* frameG[30];
	IplImage* frame2[30];
	IplImage* frame2G[30];
	IplImage* frame3[30];
	IplImage* frame3G[30];
	IplImage* frame4G[30];
	IplImage* frame4GG[30];
	double growth[2]={0,0};
	int speedcounter=0;
	int splitcheck=0,split2 = 0,smokechecker=0;
	double area3[30],spdgr;
	void countourarea(double area1);
	void colourtesting(IplImage*frameb);
	CvSeq* contour2;
	CvSeq* contour;
	CvMemStorage* storage;
	//rect
	vector<CvRect> Recty;
	vector<CvRect> Rectx;
	void contourprocess(IplImage*frameb,IplImage*framec,CvSeq* contour,double area1);
	void ghosteliminator(IplImage*frameb,IplImage*framec);
	void centertrack(IplImage* frameb);
	void splitmergegrowshrink();
	void output(IplImage*frameb);
	struct rectinfo
		{
		int coordinatex;//center of the rectangle
		int coordinatey;
		int height;
		int width;
		int smokeval;
		};
	vector<rectinfo> xyhw,xyhwpast;
	vector< vector< int > > dynamicArray;
	vector< vector< int > > summon;
	int var=0;//for output of images
	int pkarray[100000];
	int pkcount=0;
	int main(int argc, char* argv[])//MainProgram
{
	int pkfire=0;
	int b=0,a=0,d=0;
	//specify the input
	CvCapture* capture;
	int input=0,bfv = 0,counter = 0;
	double mean=10;
	double area1 = 0;
	cout<<"Please specify input\nvideo(1) or webcam(2)?";
	cin>> input;
	if (input == 1)
		{
		/*video1*/
		char filename[255];
		memset(filename,0,255);
		cout<<"Please key in the adress to the video\n";
		cin>>filename;
		capture = cvCreateFileCapture( filename );
		}
	if (input == 2)
	{
		capture =cvCreateCameraCapture(0);
	}
	//declare stuff
	IplImage* start;
	IplImage* edges[30];
	IplImage* edge[30];
	IplImage* remainder[30];
	IplImage* edgesC[30];
	IplImage* edgeC[30];
	IplImage* remainderC[30];
	start = cvQueryFrame(capture);
	//initialize images?
	for (int counter =0;counter<30;counter ++)
		{
		frame [counter] = NULL;
		frameG [counter] = NULL;
		frame2 [counter] = NULL;
		frame2G [counter] = NULL;
		frame3 [counter] = NULL;
		frame3G [counter] = NULL;
		frame4G [counter] = NULL;
		frame4GG [counter] = NULL;
		edges[counter] = NULL;
		edge[counter] = NULL;
		remainder[counter] = NULL;
		edgesC[counter] = NULL;
		edgeC[counter] = NULL;
		remainderC[counter] = NULL;
		}
//grab image to begin working
//--------------------------------------------------------------------------------------
		cvNamedWindow( "FireDetect", CV_WINDOW_AUTOSIZE );
		CvMemStorage* storage=NULL;
		CvMemStorage* storage2=NULL;
//imageCapture and process to said images

while(1)
	{
		if(storage==NULL)storage = cvCreateMemStorage(0);
		if(storage2==NULL)storage2 = cvCreateMemStorage(0);
		a=0;
		//IplImage* tImg = cvQueryFrame( capture );
		//frame[a] = tImg;
		frame[a] = cvQueryFrame( capture );
		IplImage* _img= cvQueryFrame( capture );
		if(_img==NULL)break;
		frame[a]= cvCreateImage(cvGetSize(_img),_img->depth,_img->nChannels);
		frameG[a]=cvCreateImage(cvGetSize(_img),8,1);
		frame2[a]=cvCreateImage(cvGetSize(_img),_img->depth,_img->nChannels);
		frame2G[a]=cvCreateImage(cvGetSize(_img),8,1);
		frame3[a]=cvCreateImage(cvGetSize(_img),_img->depth,_img->nChannels);
		frame3G[a]=cvCreateImage(cvGetSize(_img),8,1);
		frame4G[a]=cvCreateImage(cvGetSize(_img),8,1);
		frame4GG[a]=cvCreateImage(cvGetSize(_img),8,1);
		edges[a] = cvCreateImage(cvGetSize(_img),8,1);
		remainder[a] = cvCreateImage(cvGetSize(_img),8,1);
		edge[a] = cvCreateImage(cvGetSize(_img),8,1);
		edgesC[a] = cvCreateImage(cvGetSize(_img),_img->depth,_img->nChannels);
		remainderC[a] = cvCreateImage(cvGetSize(_img),_img->depth,_img->nChannels);
		edgeC[a] = cvCreateImage(cvGetSize(_img),_img->depth,_img->nChannels);
		cvCopy(_img,frame[a]);
		if( !frame ) break;
		//---------------------------------------------------------------------------------
		cvShowImage( "FireDetect", frame[a] );
		char c = cvWaitKey(33);
		if( c == 27 ) break;
		cvCvtColor(frame[a], frameG[a], CV_BGR2GRAY);
		//findmean
		CvScalar avg = cvAvg(frameG[a]);
		mean =(avg.val[0]/6);
		//release
		if(frame[29])
		cvReleaseImage(&frame[29]);
		if(frameG[29])
		cvReleaseImage(&frameG[29]);
		if(frame2[29])
		cvReleaseImage(&frame2[29]);
		if(frame2G[29])
		cvReleaseImage(&frame2G[29]);
		if(frame3[29])
		cvReleaseImage(&frame3[29]);
		if(frame3G[29])
		cvReleaseImage(&frame3G[29]);
		if(frame4G[29])
		cvReleaseImage(&frame4G[29]);
		if(frame4GG[29])
		cvReleaseImage(&frame4GG[29]);
		if (edges[29])
			{ 
				cvReleaseImage(&edges[29]);
				cvReleaseImage(&edge[29]);
			}
		if (remainder[29])
		cvReleaseImage(&remainder[29]);
		if (edgesC[29])
			{ 
				cvReleaseImage(&edgesC[29]);
				cvReleaseImage(&edgeC[29]);
			}
		if (remainderC[29])
		cvReleaseImage(&remainderC[29]);
		//move stuff by step
		for ( int counter2=29;counter2> 0;counter2--)
			{
				frame[counter2]= frame [(counter2-1)];
				frameG[counter2] = frameG [(counter2-1)];
				frame2[counter2]= frame2 [(counter2-1)];
				frame2G[counter2] = frame2G [(counter2-1)];
				frame3[counter2]= frame3 [(counter2-1)];
				frame3G[counter2]= frame3G [(counter2-1)];
				frame4G[counter2] = frame4G [(counter2-1)];
				frame4GG[counter2] = frame4GG [(counter2-1)];
				edges[counter2] = edges[(counter2 - 1)];
				remainder[counter2] = remainder[(counter2 - 1)];
				edge[counter2] = edge[(counter2 - 1)];
				edgesC[counter2] = edgesC[(counter2 - 1)];
				remainderC[counter2] = remainderC[(counter2 - 1)];
				edgeC[counter2] = edgeC[(counter2 - 1)];
			}
		//BufferValue
		//if (bfv<=50)
		bfv++;
		//let b and a be a constant with difference
		a=0;
		b=15;
		d=24;
		//compare to previous frame if any,note b is the oldest frame and a is newest
			if ( bfv >=26)
				{
					cvSub(frame[b],frame[a],frame2[a]);
					cvSub(frame[5],frame[a],frame3[a]);
					//need to compare mean
					cvCvtColor(frame2[a],frame2G[a],CV_BGR2GRAY);
					cvCvtColor(frame3[a],frame4G[a],CV_BGR2GRAY);
					cvThreshold(frame2G[a],frame3G[a],mean,255,CV_THRESH_BINARY);
					cvThreshold(frame4G[a],frame4GG[a],mean,255,CV_THRESH_BINARY);
					cvSmooth(frame3G[a],frame3G[a],CV_MEDIAN);
					cvSmooth(frame4GG[a],frame4GG[a],CV_MEDIAN);
					cvDilate(frame3G[a],frame3G[a],NULL,2);
					cvErode(frame3G[a],frame3G[a],NULL,2);
					cvDilate(frame4GG[a],frame4GG[a],NULL,2);
					cvErode(frame4GG[a],frame4GG[a],NULL,2);
					cvFindContours(frame3G[a],storage,&contour,sizeof(CvContour),CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
					cvFindContours(frame4GG[d],storage2,&contour2,sizeof(CvContour),CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
					//DO NOT REMOVE ANYTHING ABOVE THIS POINT
					//start of tracking corner (edges) in image?
					cvDilate(frameG[a],edges[a],NULL,1);
					cvErode(edges[a],edges[a],NULL,1);
					cvCanny(edges[a],edges[a],128,255);
				
					cvThreshold(edges[a],edges[a],128,8,CV_THRESH_BINARY);
					//colouring
					cvConvertScale(frame[a],edgesC[a],0.00333203125);
					if (edges[29]!=NULL)
						{
							for (int counter=29;counter>0;counter--)
							cvAdd(edges[counter],edges[(counter-1)],remainder[(counter)]);
							for (int counter=29;counter>0;counter--)
							cvAdd(remainder[counter],remainder[(counter-1)],remainder[counter-1]);
							cvThreshold(remainder[a],remainder[a],250,255,CV_THRESH_BINARY);
							//subtract contour curren from contour background
							cvSub(remainder[a],frameG[a],edge[a]);
							//endcontourbackground
							//trying colour background image
							for (int counter=29;counter>0;counter--)
								cvAdd(edgesC[counter],edgesC[(counter-1)],remainderC[(counter)]);
							for (int counter=29;counter>0;counter--)
								cvAdd(remainderC[counter],remainderC[(counter-1)],remainderC[counter-1]);
							//trying colour background image /end
						}
					cvThreshold(edge[1],edge[2],1,255,CV_THRESH_BINARY);
					double SE=0,RE=0;
					CvScalar Sedge = cvSum(remainder[a]);
					CvScalar Redge = cvSum(edge[a]);
					SE =(Sedge.val[0]);
					RE =(Redge.val[0]);
				
					IplImage* frameb = frame[15];
					IplImage* framec = frame[16];
					contourprocess(frameb,framec,contour,area1);
//--------------------------------------------------pk--------------------------------------------------------------------------------------
					//cout<<pkarray[pkcount-1]<<endl;
					if(pkcount>=12)
						{
						int pk123=pkcount-1;
						if(pkarray[pk123-1]>=10 && pkarray[pk123-2]>=10 && pkarray[pk123-3]>=10 && pkarray[pk123-4]>=10 && pkarray[pk123-5]>=10 && pkarray[pk123-6]>=10 && pkarray[pk123-7]>=10 && pkarray[pk123-8]>=10 && pkarray[pk123-8]>=10 && pkarray[pk123-9]>=10 && pkarray[pk123-10]>=10)
							{
							cout <<"Avg value = " <<exp_sum/exp_count <<endl;							
							cout<<endl<<"FIRE-DETECTED"<<endl;
							return 0;
							pkfire=123;
							break;
							}
						}
//--------------------------------------------------pk--------------------------------------------------------------------------------------
					//clearing stuff,
					if (contour!=NULL)
					cvClearSeq(contour);
					if (contour2!=NULL)
					cvClearSeq(contour2);
					cvClearMemStorage(storage);
					cvClearMemStorage(storage2);

				}
	}//while loop end
		if(pkfire=123)
		{
			//cvReleaseMemStorage(&storage);
			//cvReleaseMemStorage(&storage2);
			//cvReleaseCapture( &capture );
			cvDestroyWindow( "FireDetect" );
			cvDestroyWindow( "contours" );
			cvDestroyWindow( "greymove" );
			cvNamedWindow( "FIRE", 1 );
  			IplImage *img = cvCreateImage( cvSize( 900, 400 ), IPL_DEPTH_8U, 4 );
  			CvFont font;
  			double hScale = 2.5;
  			double vScale = 2.5;
  			int lineWidth = 10;
  			cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC,
              		hScale, vScale, 0, lineWidth );
			cout <<"Avg value = " <<exp_sum / exp_count <<endl;  			
			cvPutText( img, "NO FIRE-DETECTED", cvPoint( 200, 200 ), &font,
             		cvScalar(0,0,255) );
  			cvShowImage( "FIRE", img);
  			cvWaitKey();
			//cvDestroyAllWindows();
		}
		return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------
//void stuff here...
struct garupa{
	vector<CvRect>grouped;
	CvRect overallRect,overallEnd;
	};
	vector<CvRect> allcontours;
	vector<int>decopy;
	vector<garupa> grouper;
	rectinfo xyhw_;
	int Contcounter=0;
	int area1=0;
	CvScalar s,r;
	int framecounter = 0;
	struct gray{//colour checking gray
		double grey;
		int positiongreyx,positiongreyy,xspot,yspot,xlength,ylength;
		CvScalar Outline;
	};
	vector<gray> greyness;
	vector<gray> greynessp;
	gray gravy;
	garupa objGrp;
	int errorchecker = 0;
	vector<gray> flamechecker;
	vector<gray> flameprevious;
	int experimentcounter = 0;

	double onecontarea;
	vector<double> contarea,contareap;
	struct individualcontourinformation
		{
		vector<int> contournumber,colourgrey,colourred,splitmerge,splitmerge2,growshrink,mimic,sizemin,smokeval;
		}unit;

void contourprocess(IplImage* frameb,IplImage* framec,CvSeq *contour,double area1)//ContourProcess
	{
		int b= 15,a=0;
		double Side, Adjacent, Hypertenuse;
		//finding contour
		if (contour!=NULL)
		cvDrawContours(frameb,contour,cvScalar(0,255,0,0),cvScalar(255,0,0,0),255);
		char imagestring[255];
		memset(imagestring,0,255);
	
		cvNamedWindow("contours");
		cvShowImage("contours",frameb);
		
		for( CvSeq* e = contour; e!= 0; e = e->h_next )
			{
				onecontarea = fabs(cvContourArea(e));
				area1 = area1 + onecontarea;
				CvRect z=cvBoundingRect(e,0);
				//---------------------------------------------------------------------------------------
				// cvDrawRect(frameb,cvPoint(z.x,z.y),cvPoint(z.x+z.width,z.y+z.height),cvScalar(255,0,0,0));
				Recty.push_back(z);
					Contcounter++;
				contarea.push_back (onecontarea);
				// Contour Area, a void function
				// countourarea( area1);
				allcontours.push_back(z);
			}
		int i = 1;
		for( int j=0 ;j<Contcounter; j++)
			{
				unit.contournumber.push_back(i);
				i++;
			}
		//Grouping
		float totalareapixel=0;
		for( int i=0; i<Contcounter;i++)
			{
				xyhw_.coordinatex =Recty[i].x + (Recty[i].width/2);
				xyhw_.coordinatey =Recty[i].y + (Recty[i].height/2);
				xyhw_.height = Recty[i].height;
				xyhw_.width = Recty[i].width;
				xyhw.push_back(xyhw_);
				float widthx,widthy;
				widthx=Recty[i].x + Recty[i].width;
				widthy=Recty[i].y + Recty[i].height;
				float counterarea=widthx*widthy;
				totalareapixel=totalareapixel+counterarea;
			}
		cout<<endl<<"counter area"<<totalareapixel<<endl;
		
		
		// contour colour
		colourtesting(frameb);
		splitmergegrowshrink();
		ghosteliminator(frameb,framec);
		output(frameb);
		centertrack(frameb);
		
		//important resizing
		decopy.resize(xyhw.size());
		for(int i =0;i<(int)decopy.size();i++)
			{
				decopy[i]=-1;
			}
		double range,reverserange;
		for (int i=0;i<Contcounter;i++)
			{
				if(decopy[i]<0)
					{
						objGrp.grouped.push_back(allcontours[i]);
					}
				range = (xyhw[i].height)*(xyhw[i].height)+(xyhw[i].width)*(xyhw[i].width);
				for (int j=i+1;j<Contcounter;j++)
					{
						reverserange = (xyhw[j].height)*(xyhw[j].height)+(xyhw[j].width)*(xyhw[j].width);
						Side = abs(xyhw[i].coordinatex-xyhw[j].coordinatex);
						Adjacent = abs(xyhw[i].coordinatey-xyhw[j].coordinatey);
						Hypertenuse = sqrt(((Side*Side)/4) + ((Adjacent*Adjacent)/4)); //<--modify later
						if(Hypertenuse<=(sqrt(range)))
							{
								if(decopy[i]<0)
									{
										if (decopy[i]<decopy[j])
											{
												decopy[i]=decopy[j];
											}
										else
											{
												objGrp.grouped.push_back(allcontours[j]);
												decopy[i] = i;
												decopy[j] = i;
											}
									}
						else if(decopy[j]<0)
							{
								if (decopy[j]<decopy[i])
									{
										decopy[j] = decopy[i];
									}
								else
									{
										objGrp.grouped.push_back(allcontours[j]);
										decopy[i] = i;
										decopy[j] = i;
									}
							}
					}
					else if(Hypertenuse<=sqrt(reverserange))
						{
							objGrp.grouped.push_back(allcontours[j]);
							if (decopy[j]<decopy[i])
								{
									if(decopy[j]>=0)
										decopy[i] = decopy[j];
									else
										decopy[j] = decopy[i];
								}
							else if (decopy[j]>decopy[i])
								{
									if(decopy[i]>=0)
										decopy[j] = decopy[i];
									else
										decopy[i] = decopy[j];
								}
						}
					}
						objGrp.overallRect.x = INT_MAX;
						objGrp.overallRect.y = INT_MAX;
						objGrp.overallEnd.x = 0;
						objGrp.overallEnd.y = 0;
					if(objGrp.grouped.size()>0)
						grouper.push_back(objGrp);
					}
					for (int i=0;i<(int)grouper.size();i++)
						{
							for (int j=0;j<(int)grouper[i].grouped.size();j++)
								{
									if(grouper[i].grouped[j].x < grouper[i].overallRect.x)
										{
											grouper[i].overallRect.x = grouper[i].grouped[j].x;
										}
									if(grouper[i].grouped[j].y < grouper[i].overallRect.y)
										{
											grouper[i].overallRect.y = grouper[i].grouped[j].y;
										}
									if(grouper[i].grouped[j].x + grouper[i].grouped[j].width > grouper[i].overallEnd.x)
										{
											grouper[i].overallEnd.x = grouper[i].grouped[j].x+grouper[i].grouped[j].width;
										}
									if(grouper[i].grouped[j].y + grouper[i].grouped[j].height > grouper[i].overallEnd.y)
										{
											grouper[i].overallEnd.y = grouper[i].grouped[j].y+ grouper[i].grouped[j].height;
										}
								}
						}
					//grouping done!(don't remove the above)
					//scan all value, take last similar value.
					vector<int> lumped;
					vector<int>::iterator leong;
					vector<garupa> lumpy;
					for (int i=0;i<(int)grouper.size();i++)
						{
							lumped.push_back ( decopy[i]);
						}
					sort(lumped.begin(),lumped.end());
					leong = unique( lumped.begin(), lumped.end() );
					lumped.erase( leong, lumped.end() );
					for(int i=0; i<(int)lumped.size();i++)
						{
						//while (decopy[j]!=lumped[i])
					for (int j=((int)grouper.size()-1);j>0;j--)
						{
						//find value of lumped from behind
							if (decopy[j]==lumped[i])
								{
									lumpy.push_back (grouper[j]);
									break;
								}
						}
					}
					for (int i=0;i<(int)lumpy.size();i++)
						{
							if (lumped[i]!=-1)//remove loners
								{
									for (int j=0;j<(int)lumpy[i].grouped.size();j++)
										{
//--------------------------------------------------------------------------------------------------------------------------------
											cvDrawRect(frameb,
											cvPoint(lumpy[i].overallRect.x,
											lumpy[i].overallRect.y),
											cvPoint(lumpy[i].overallEnd.x,
											lumpy[i].overallEnd.y),
											//gravy.Outline);
											cvScalar(0,255,255));
										}
								}
						}
					//splitmerge
					double totalarea=0,pastotarea=0;
					for (int i=0;i<(int)lumpy.size();i++)
						{
							totalarea = contarea[i]+totalarea ;
								for(int j=0;j<(int)xyhwpast.size();j++)
									{
										//previous square same location area of contour in contact. find xyhwpast inside
										if((xyhwpast[j].coordinatex <= lumpy[i].overallRect.x)&&
										(xyhwpast[j].coordinatex >= lumpy[i].overallEnd.x)&&
										(xyhwpast[j].coordinatey <= lumpy[i].overallRect.y)&&
										(xyhwpast[j].coordinatey >= lumpy[i].overallEnd.y ))
											{ //find the area.
												pastotarea= pastotarea+contareap[j];
											}
									}
						//comparepast to present,
						//if (pastotarea<totalarea)
						//{
						//cout<<"totalareal growth";
						//}else {
						//cout<<"totalareal shrinkage";
						//}
						}
						for (int i=0;i<(int)grouper.size();i++)
							{
								grouper[i].grouped.clear();
							}
						grouper.clear();
						//for (i=0; i <groupedcontours.size ;i++)
						cvNamedWindow("greymove");
						cvShowImage("greymove",frameb);
						decopy.clear();
						xyhwpast.clear();
						xyhwpast = xyhw;
						xyhw.clear();
						allcontours.clear();
						objGrp.grouped.clear();
						lumped.clear();
						lumpy.clear();
						Contcounter = 0;
						if (contour!=NULL)
							{
								cvClearSeq(contour);
							}
						//GroupingEnd
						split2=0;
						Rectx.clear();
						Rectx = Recty;
						Recty.clear();
						contareap.clear();
						contareap = contarea;
						contarea.clear();
						unit.colourgrey.clear();
						unit.contournumber.clear();
						unit.growshrink.clear();
						unit.splitmerge.clear();
						framecounter++;
						}
void colourtesting(IplImage*frameb)// Colour
{
	for( int i=0;i<Contcounter;i++)
		{
		s = cvGet2D(frameb, xyhw[i].coordinatey ,xyhw[i].coordinatex);
		if (s.val[2]=s.val[1] = s.val[0])
			{
			gravy.grey = s.val[2];
			gravy.positiongreyx = xyhw[i].coordinatex;
			gravy.positiongreyy = xyhw[i].coordinatey;
			gravy.xspot = (xyhw[i].coordinatex-(xyhw[i].width/2));//point left top corner of rectangle
			gravy.yspot = (xyhw[i].coordinatey-(xyhw[i].height/2));
			gravy.xlength = xyhw[i].width;
			gravy.ylength = xyhw[i].height;
			gravy.Outline = s;
			greyness.push_back(gravy);
			unit.colourgrey.push_back (1);
			}
			else
			{
			unit.colourgrey.push_back (0);
			}
			//flame test
			CvScalar areat;
			double areaT=(xyhw[i].height*xyhw[i].width),areatt=0;
			double areaTT = 0;
			for(int j=0;j<gravy.xlength ;j++)
			{
				for (int k=0;k<gravy.ylength;k++)
				{
				areat =cvGet2D(frameb, gravy.yspot +k ,gravy.xspot +j);
				if ((areat.val[2]>areat.val[1])&&(areat.val[2]>areat.val[0])&&(areat.val[1]>areat.val[0])&&(areat.val[2]>70))
					{
					areatt++;
					}
				}
			}
			areaTT = (areatt/areaT);
			if (areaTT>0.6)
				{
				unit.colourred.push_back(1);
				}
				else
					{
					unit.colourred.push_back(0);
					}
			if (areaT>6)
				{
				for(int j=0;j<gravy.xlength ;j++)
				{
					for (int k=0;k<gravy.ylength;k++)
					{
					areat =cvGet2D(frameb, gravy.yspot +k ,gravy.xspot +j);
					if ((areat.val[2]>areat.val[1])&&(areat.val[2]>areat.val[0])&&(areat.val[1]>areat.val[0])&&(areat.val[2]>70))
						{
						areatt++;
						}
					}
				}
				areaTT = (areatt/areaT);
		
			}
			flameprevious.clear();
			flameprevious = flamechecker;
			flamechecker.clear();
			//flame test end
			if (greynessp.size()>0)//make sure it exist
				{
				for (int j=0; j < (int)greynessp.size();j++)
					{
					r = cvGet2D(frameb,greynessp[j].positiongreyy,greynessp[j].positiongreyx);
					if ((r.val[0]=r.val[1] = r.val[2])&&(r.val[2]<250))
						{
						if((r.val[2]>(greynessp[j].grey))||(r.val[2]<(greynessp[j].grey)))
							{
							//cout<<"g";
							//check to see if currently have any contours on it?add range
							}
						}
						else
							{
						if ((r.val[0]==r.val[1])||(r.val[0]==r.val[2])||(r.val[1]==r.val[2]))
							{
							//cout<<"?"<<endl;
						}
					}
				}
			}
		}
		greynessp.clear();
		greynessp=greyness;
		greyness.clear();
	}
//-----------------------------------------------------------------------------------------------------------
	void splitmergegrowshrink()//input is current frame, previous frame,unit?,
		{
		int splitchecker = 0,mergechecker = 0;
		int A=0,B=0,C=0,D=0,A1=0,B1=0,C1=0,D1=0;
		for (int i=0;i<Contcounter;i++)
			{
			mergechecker = 0;
			splitchecker = 0;
			A = xyhw[i].coordinatex-(xyhw[i].width/2);
			B = xyhw[i].coordinatex+(xyhw[i].width/2);
			C = xyhw[i].coordinatey-(xyhw[i].height/2);
			D = xyhw[i].coordinatey+(xyhw[i].height/2);
			//compare to all previous squares?
			for(int j=0;j<(int)xyhwpast.size();j++)
				{
				//problem
				//if (xyhwpast[j].smokeval==1)
				//{
				//smokechecker++;
				//}
				A1 = xyhwpast[j].coordinatex-(xyhwpast[j].width/2);
				B1 = xyhwpast[j].coordinatex+(xyhwpast[j].width/2);
				C1 = xyhwpast[j].coordinatey-(xyhwpast[j].height/2);
				D1 = xyhwpast[j].coordinatey+(xyhwpast[j].height/2);
				if(((A1>=A)||(B1>A)&&((A1<=B)||(B1<=B)))&&(((C1>=C)||(D1>=C))&&((C1<=D)||(D1<=D))))//inside point(dont care which part? double check for certaincy later
					{
					mergechecker++;
					if (xyhwpast[j].smokeval==1)
						{
						smokechecker++;
						}
					}
				if(((A>=A1)||(B>A1)&&((A<=B1)||(B<=B1)))&&(((C>=C1)||(D>=C1))&&((C<=D1)||(D<=D1))))//inside point(dont care which part? double check for certaincy later
					{
					splitchecker++;
					if (xyhwpast[j].smokeval==1)
						{
							smokechecker++;
						}
					}
				}
				//check for merge
				//yes ->count contours, Greater than 1?
				if (mergechecker>1)
					{
					unit.splitmerge.push_back(1);
					}
				else
					{
					unit.splitmerge.push_back(0);
					}
				//check for split
				//compare previous,any contour outside(part if contour)?
				//yes -> note outside contour corners,compare current frame, no frame contour>1?
				if (splitchecker>1)
					{
					unit.splitmerge2.push_back(1);
					}
				else
					{
					unit.splitmerge2.push_back(0);
					}
				if (smokechecker>0)
					{
					unit.smokeval.push_back(1);
					}
				else
					{
					unit.smokeval.push_back(0);
					}
				char m=0;
				if (mergechecker ==1&& splitchecker ==0)//not greater than one? growth occured(check growth factor
				m = 'g';
				if (splitchecker == 1&& mergechecker==0)//shrink occured, check shrink factor?
				m = 's';
				if(splitchecker ==0&&mergechecker==0)
				m = 'n';
				smokechecker=0;
				switch (m)
				{
				case ('g')://not greater than one? growth occured(check growth factor
					{
					unit.growshrink.push_back(1);
					break;
					}
				case ('s')://shrink occured, check shrink factor?
					{
					unit.growshrink.push_back (2);
					break;
					}
				case ('n'):
					{
					unit.growshrink.push_back(0);//size same or does not exist
					break;
					}
				default:
				unit.growshrink.push_back(4);
			}
	}
}
	void output(IplImage*frameb)
		{
		if (framecounter==0)
		{
			summon.resize(frameb->width,vector<int>(frameb->height));
			for(int i = 0;i < (int)summon.size();++i)
			{
				for(int j = 0;j < (int)summon[i].size();++j)
				{
				summon[i][j] = 0;//set 0
				}
			}
		}
		int smokiness=0;
		for (int i = 0; i <(int)unit.contournumber.size();i++)
			{
			if (unit.sizemin[i]==0&&unit.mimic[i]==0)
				{
				if ((unit.colourgrey[i]==1)||(unit.colourred[i]==1))
					{
					if (unit.growshrink[i]!=0)
					smokiness++;
					if (unit.splitmerge[i]==1)
					smokiness++;
					if (unit.splitmerge2[i]==1)
					smokiness++;
					if (smokiness>1)
						{
						xyhw[i].smokeval =1;
						unit.smokeval[i] = 1;
						}
					}
				}
			smokiness=0;
			int j=0;
			if((xyhw[i].smokeval==1))
			{
	//-----------------------------------------------------------------------------------------------------------------------------
			cvDrawRect(frameb,cvPoint((xyhw[i].coordinatex-(xyhw[i].width/2)),(xyhw[i].coordinatey)-(xyhw[i].height/2)),
			cvPoint((xyhw[i].coordinatex+(xyhw[i].width/2)),(xyhw[i].coordinatey)+(xyhw[i].height/2)),
			cvScalar(255,0,0,0));
			experimentcounter++;
			if (experimentcounter>2)
				{
				cout<<"\a";
				}
			for(int a = (xyhw[i].coordinatex-(xyhw[i].width/2));a < (xyhw[i].coordinatex+(xyhw[i].width/2));++a)
				{
				for(int b = (xyhw[i].coordinatey)-(xyhw[i].height/2);b < (xyhw[i].coordinatey)+(xyhw[i].height/2);++b)
					{
					summon[a][b]++;
					}
				}
//-----------------------------------------------------------------------------------------------------------------
//cvDrawContours(frameb,c,cvScalar(255,0,0,0),cvScalar(0,0,0,0),1,2,8 );
			}//else{cvDrawContours(frameb,c,cvScalar(255,255,0,0),cvScalar(0,0,0,0),1,1,8 );}
		}
	//----------------------------------------------------------------------------------------------------------------------------
			//fprintf(fp,"%d\t",experimentcounter);
			errorchecker++;
			smokechecker=0;
			unit.colourgrey.clear();
			unit.colourred.clear();
			unit.contournumber.clear();
			unit.growshrink.clear();
			unit.splitmerge.clear();
			unit.splitmerge2.clear();
			unit.mimic.clear();
			unit.sizemin.clear();
			unit.smokeval.clear();
	}
void ghosteliminator(IplImage*frameb,IplImage*framec)
	{
		CvScalar areat,areap;
		for (int i=0;i<(int)xyhw.size();i++)
		{
			double areaT=(xyhw[i].height*xyhw[i].width),areatt=0;
			if (areaT>6)//size filter, if too small return 1
			{
			unit.sizemin.push_back(0);}else{unit.sizemin.push_back(1);
			}
			double areaTT = 0;
			for(int j=0 ;j<gravy.xlength ;j++)
			{
				for (int k=0 ;k<gravy.ylength;k++)
				{
				areat=cvGet2D(frameb, gravy.yspot+k ,gravy.xspot+j);
				areap=cvGet2D(framec, gravy.yspot+k ,gravy.xspot+j);
				if (((areat.val[0]==areap.val[0])&&(areat.val[1]==areap.val[1])&&(areat.val[2]==areap.val[2])))
					{
					areatt++;
					}
				}
			}
			areaTT = (areatt/areaT);
			if (areaTT>0.7)
			{
			unit.mimic.push_back(1);
			}
			else unit.mimic.push_back(0);
			}
	}

void centertrack(IplImage* frameb)
{
	//----------------------------------------------------------------------------------------------------------------------------
	 ofstream fout;
         fout.open("output.txt",ios::app);
	//----------------------------------------------------------------------------------------------------------------------------
	//track centre of fire... via density?(smoke central point)
	if (framecounter==0)
	{
		dynamicArray.resize(frameb->width,vector<int>(frameb->height));
		for(int i = 0;i < (int)dynamicArray.size();++i)
		{
		for(int j = 0;j < (int)dynamicArray[i].size();++j)
		{
		dynamicArray[i][j] = 0;//set 0
		}
		}
	}
		// for each contour pixel add one.
		//for (int k=0;k<(int)xyhw.size();k++)
		//{
		// for(int i=gravy.xspot;i<(gravy.xspot +gravy.xlength) ;i++)
		// {
		// for (int j=gravy.yspot ;j<(gravy.yspot+gravy.ylength);j++)
		// {
		// dynamicArray[i][j]=dynamicArray[i][j]+1000;
		// }
		// }
		//}
		int smokecount=0;
		for(int i=0;i<(int)dynamicArray.size();i++)
			{
			for (int j=0;j<(int)dynamicArray[i].size();j++)
			{
				if (summon[i][j]>0)
				{
				smokecount++;
				}
			}
		}
//---------------------------------------------------------------------------------------------------------------------------------
			//cout<<endl<<experimentcounter<<endl;
			//---------------------------------------------------------------------------------------------------------
			fout<<experimentcounter<<endl;

			exp_count++;
			exp_sum +=experimentcounter;

			pkarray[pkcount]=experimentcounter;
			pkcount++;
			//---------------------------------------------------------------------------------------------------------
			experimentcounter=0;
			for(int i=0;i<(int)dynamicArray.size();i++)
			{
				for (int j=0;j<(int)dynamicArray[i].size();j++)
				{
				if (summon[i][j]>0)
				{
				dynamicArray[i][j]=dynamicArray[i][j]+1000;
				summon[i][j]=0;
				}
				}
			}
			//see densest area of contour, point is suspected to be smoke emitting point.
			for(int i=0;i<(int)dynamicArray.size();i++)
				{
				for(int j=0;j<(int)dynamicArray[i].size();j++)
				{
					if (dynamicArray[i][j]>0)
					{
					dynamicArray[i][j]=7*dynamicArray[i][j]/10;
					}
				}
			}
			//output side
			for(int i=0;i<(int)dynamicArray.size();i++)
				{
				for(int j=0;j<(int)dynamicArray[i].size();j++)
					{
					if (dynamicArray[i][j]>3000)//test 10 first
					{
					CvPoint pt ={i,j};
					((uchar*)(frameb->imageData + frameb->widthStep*pt.y))[pt.x*3] = 255;
					((uchar*)(frameb->imageData + frameb->widthStep*pt.y))[pt.x*3+1] = 0;
					((uchar*)(frameb->imageData + frameb->widthStep*pt.y))[pt.x*3+2] = 255;
					}
				}
			}
}
