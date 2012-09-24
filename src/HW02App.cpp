/**
 * @file HW02App.cpp
 * CSE 274 - Fall 2012
 *
 * @author Jordan Komnick
 * @date 2012-09-05
 *
 * @note This file is (c) 2012. It is licensed under the 
 * CC BY 3.0 license (http://creativecommons.org/licenses/by/3.0/),
 * which means you are free to use, share, and remix it as long as you
 * give attribution. Commercial uses are allowed.
 *
 * @note Some code and theories come from https://github.com/brinkmwj/HW01 and https://github.com/jordankomnick/HW01Part2
 * @note The code for displaying text is from the TextureFont sample included with the Cinder library
 *
 * @note This application contains an implementation of a circular, doubly-linked list that contains a number of circles.
 * It satisfies goals A, B, C, E, H, and I
 */

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Resources.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Text.h"
#include <iostream>
#include <conio.h>

using namespace ci;
using namespace ci::app;
using namespace std;

class Circle;
class LinkedList;

class HW02App : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void keyDown(KeyEvent event);
	void update();
	void draw();
	void prepareSettings(Settings* settings);
	
	private:
	Surface* mySurface_;
	uint8_t* dataArray;
	bool show_info;
	bool do_reverse;
	bool do_push_to_front;
	int x_clicked;
	int y_clicked;
	LinkedList* linked_list;

	//window width and height and texture size
	static const int kAppWidth = 800;
	static const int kAppHeight = 600;
	static const int kTextureSize = 1024;

	//texture font variables
	Font mFont;
	gl::TextureFontRef	mTextureFont;
};

//contains data for a circle
class Circle{
public:
	Circle(int r, int x, int y, Color8u c);
	int radius;
	int center_x;
	int center_y;
	Color8u color;
	void draw(uint8_t* pixels, int kAppWidth, int kAppHeight, int kTextureSize);
};

Circle::Circle(int r, int x, int y, Color8u c){
	radius = r;
	center_x = x;
	center_y = y;
	color = c;
}

//each node contains a Circle, next and previous nodes, and can contain a child node
class Node{
public:
	Node(Circle* new_data);
	void addChild(Node* new_child);
	Node* next_;
	Node* previous_;
	Node* child;
	Circle* data;
};

Node::Node(Circle* new_data){
	next_ = NULL;
	previous_ = NULL;
	child = NULL;
	data = new_data;
}

void Node::addChild(Node* new_child)
{
	child = new_child;
}

//a doubly linked circular list, with the sentinel acting as a beginning/end
class LinkedList{
public:
	LinkedList();
	Node* sentinel;
	void insertAt(Node* location, Circle* data);
	void remove(Node* location_before);
	void reverse();
};

LinkedList::LinkedList(){
	sentinel = new Node(NULL);
	sentinel -> next_ = sentinel;
	sentinel -> previous_ = sentinel;
}
void LinkedList::insertAt(Node* location, Circle* data){
	Node* temp = new Node(data);
	temp -> next_ = location -> next_;
	location -> next_ = temp;
	temp -> previous_ = location;
}

void LinkedList::remove(Node* location_before){
	Node* temp = location_before -> next_ -> next_;
	location_before -> next_ = temp;
	temp -> previous_ = location_before;
}

//app sometimes crashes when this is called, unsure wh
//Reverses the order of the list
void LinkedList::reverse()
{
	Node* curr = sentinel;
	do
	{
		Node* temp = curr -> next_;
		curr -> next_ = curr -> previous_;
		curr -> previous_ = temp;
		curr = curr -> previous_;
	}
	while(curr != sentinel);
}

void HW02App::prepareSettings(Settings* settings){
	(*settings).setWindowSize(kAppWidth,kAppHeight);
	(*settings).setResizable(false);
}

void HW02App::setup()
{
	mySurface_ = new Surface(kTextureSize,kTextureSize,false);
	show_info = true;
	do_reverse = false;
	do_push_to_front = false;
	mFont = Font( "Times New Roman", 24 );
	mTextureFont = gl::TextureFont::create( mFont );
}

//Stores booleans that remember keypresses
void HW02App::keyDown(KeyEvent event)
{
	if(event.getChar() == '?' || event.getChar() == '/')
	{
		if(show_info)
		{
			show_info = false;
		}
		else
		{
			show_info = true;
		}
	}
	if(event.getChar() == 'r')
	{
		do_reverse = true;
	}
	if(event.getChar() == 'p')
	{
		do_push_to_front = true;
	}
}

//Draws a given circle
void Circle::draw(uint8_t* pixels, int kAppWidth, int kAppHeight, int kTextureSize)
{
	for(int y = center_y - radius; y <= center_y + radius; y++)
	{
		for(int x = center_x - radius; x <= center_x + radius; x++)
		{
			//test for accessing array out of bounds
			if(y < 0 || x < 0 || x >= kAppWidth || y >= kAppHeight) continue;
			int dist = (int)sqrt((double)((x-center_x)*(x-center_x) + (y-center_y)*(y-center_y)));
			if(dist <= radius)
			{
					int offset = 3*(x + y*kTextureSize);
					pixels[offset] = color.r;
					pixels[offset+1] = color.g;
					pixels[offset+2] = color.b;
			}
		}
	}
}

//Mouse interaction ended up unused
void HW02App::mouseDown( MouseEvent event )
{
	x_clicked = event.getX();
	y_clicked = event.getY();
}

void HW02App::update()
{
	uint8_t* dataArray = (*mySurface_).getData();
	LinkedList* linked_list = new LinkedList();
	linked_list -> insertAt(linked_list -> sentinel, new Circle(50, 200, 200, Color8u(0, 50, 0)));
	linked_list -> sentinel -> next_ -> addChild(new Node(new Circle(30, 240, 240, Color8u(0, 100, 0))));
	linked_list -> insertAt(linked_list -> sentinel -> next_, new Circle(50, 400, 400, Color8u(50, 0, 0)));
	linked_list -> insertAt(linked_list -> sentinel -> next_ -> next_, new Circle(50, 420, 420, Color8u(0, 0, 50)));
	//reverses the list if 'r' has been pressed
	if(do_reverse)
	{
		linked_list -> reverse();
		do_reverse = false;
	}
	//takes the last object in the list and pushes it to the front, also brightens said object's color
	if(do_push_to_front)
	{
		Node* temp = linked_list -> sentinel -> next_;
		if(temp -> data -> color.r < 255-50)
		{
			temp -> data -> color.r += 50;
		}
		else
		{
			temp -> data -> color.r = 255;
		}
		if(temp -> data -> color.g < 255-50)
		{
			temp -> data -> color.g += 50;
		}
		else
		{
			temp -> data -> color.g = 255;
		}
		if(temp -> data -> color.b < 255-50)
		{
			temp -> data -> color.b += 50;
		}
		else
		{
			temp -> data -> color.b = 255;
		}
		linked_list -> remove(linked_list -> sentinel);
		Node* temp2 = linked_list -> sentinel -> next_;
		while(temp2 -> next_ != linked_list -> sentinel)
		{
			temp2 = temp2 -> next_;
		}
		linked_list -> insertAt(temp2, temp -> data);
	}
	//draws nodes in order, with child nodes being drawn immediately after parent nodes
	Node* curr = linked_list -> sentinel -> next_;
	while(curr != linked_list -> sentinel)
	{
		curr -> data -> draw(dataArray, kAppWidth, kAppHeight, kTextureSize);
		if(curr -> child != NULL)
		{
			curr -> child -> data -> draw(dataArray, kAppWidth, kAppHeight, kTextureSize);
		}
		curr = curr -> next_;
	}
}


void HW02App::draw()
{
	if(show_info)
	{
		gl::setMatricesWindow( getWindowSize() );
		gl::enableAlphaBlending();
		gl::clear( Color( 0, 0, 0 ) );
	
		std::string str("Press '?' to show/hide these instructions. Press 'p' to push the bottom shape to the front of the list.  Press 'r' to reverse the list.");
		Rectf boundsRect( 40, mTextureFont->getAscent() + 40, getWindowWidth() - 40, getWindowHeight() - 40 );

		gl::color( ColorA( 1, 0.5f, 0.25f, 1.0f ) );

		mTextureFont->drawStringWrapped( str, boundsRect );
	}
	else
	{
		gl::clear(Color(0,0,0));
		gl::draw(*mySurface_);
	}
}

CINDER_APP_BASIC( HW02App, RendererGl )
