#pragma once

#include "ofMain.h"
#include "ofxGui.h"


//Implemented based on  Emitter.h, Sprite.h from Kevin M. Smith - CS 134 SJSU

class Shape {
public:
	Shape() {}
	virtual void draw() {}
	virtual bool inside(glm::vec3 p) {
		return true;
	}


	glm::mat4 getMatrix() {

		glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
		glm::mat4 scale = glm::scale(glm::mat4(1.0), scales);

		return(trans * rot * scale);
	};

	glm::vec3 pos;
	float rotation = ofRandom(0, 360); // degrees ofRandom(0,360)
	float oriAngle = 0;
	glm::vec3 scales = glm::vec3(1, 1, 1);
	glm::vec3 headingV;
	vector<glm::vec3> verts;

	ofImage sprite;
};

class TriangleShape : public Shape {
public:
	TriangleShape() {}
	TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}
	
	bool inside(glm::vec3 p) {

		//point to object space
		//
		glm::vec3 p0 = glm::inverse(getMatrix()) * glm::vec4(p, 1);
		return (inside(p0, verts[0], verts[1], verts[2]));
	};

	bool inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		glm::vec3 v1 = glm::normalize(p1 - p);
		glm::vec3 v2 = glm::normalize(p2 - p);
		glm::vec3 v3 = glm::normalize(p3 - p);
		float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
		float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
		float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
		if (a1 < 0 && a2 < 0 && a3 < 0) return true;
		else return false;
	}


};

class Player : public TriangleShape {
public:
	Player() {}
	Player(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	
	}
	int headingLength = 75;
	void draw(bool a);
	glm::vec3 heading() ;
	void drawSprite();

	vector<glm::vec3> headingVert;
	
};


// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject : public Player{
public:
	BaseObject();
	ofVec2f trans, scale;
	float	rot;
	bool	bSelected;
	void setPosition(ofVec3f);
};

//  General Sprite class  (similar to a Particle)
//
class AgentSprite : public BaseObject {
public:
	AgentSprite();
	void draw(bool a, glm::vec3);
	float age();
	void setImage(ofImage);
	float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;
	bool showAgentSprite;
	float width, height;
	
	float GetOriAngle(glm::vec3);
};

//  Manages all Sprites in a system.  You can create multiple systems
//
class AgentSpriteSystem {
public:
	void add(AgentSprite);
	void remove(int);
	int removeNear(ofVec3f, float);
	void update(glm::vec3);
	void draw(bool a, glm::vec3);
	vector<AgentSprite> sprites;

};

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class AgentEmitter : public BaseObject {
public:
	AgentEmitter(AgentSpriteSystem*);
	AgentEmitter();
	void init();
	void draw(bool a, glm::vec3);
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(float);
	void setChildImage(ofImage);
	void setChildSize(float w, float h) { childWidth = w; childHeight = h; }
	void setImage(ofImage);
	void setRate(float);
	void update(glm::vec3);
	AgentSpriteSystem* sys;
	float rate;
	float velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	float width, height;
	float childWidth, childHeight;
};


class ofApp : public ofBaseApp{

	public:

		void checkCollisions();
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		Player ply;
		TriangleShape tri;
		bool mousep = false;
		bool dragged = false;
		glm::vec3 delta;


		ofxFloatSlider playerScale;
		ofxFloatSlider speed;
		ofxToggle showSprite;
		ofxToggle showHeading;
		ofxPanel gui;
		glm::vec3 playerScales;


		
		int numEmitters;


		ofImage defaultImage;
		bool imageLoaded;

		ofImage backgroundImage;

		int energyLevels;
	
		ofxIntSlider energyLevel;
		ofxIntSlider gameLevels;
		ofxFloatSlider rate;
		ofxFloatSlider life;
		ofxIntSlider numAgents;
		ofxIntSlider difficulty;
		ofxToggle agentShowSprite;
		ofxLabel screenSize;
		float agentSpeed;
		float gameStartTime;
		float totalgameTime;
		float time;
		bool gameOver = false;
			bool timmer;
			bool gamestared = false;
vector<AgentEmitter*> emitters;
		map<int, bool> keymap; 
		bool bHide;
		
};

