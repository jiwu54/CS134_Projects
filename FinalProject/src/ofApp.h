#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "Particle.h"
#include "ParticleEmitter.h"



class ofApp : public ofBaseApp{

	public:
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
		void dragEvent2(ofDragInfo dragInfo);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);
		void loadVbo();
		void loadExplosionVbo();
		bool checkCollisions();
		void aglCalculation();
		
		ofImage backgroundImage;
		ofEasyCam cam;
		ofCamera cam1, cam2,cam3, cam4;
		ofCamera* theCam;

		ofxAssimpModelLoader moon, lander;
		ofLight light;
		Box boundingBox, landerBounds;
		Box landingArea;
		vector<Box> colBoxList;
		bool bLanderSelected = false;
		Octree octree;
		TreeNode selectedNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;
		bool landed = false;
		bool fuel = true;

		ofxFloatSlider gravity;
		ofxToggle showHeading;
		ofxPanel gui;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bPointSelected;
		bool bHide = true;
		bool pointSelected = false;
		bool bLanderLoaded = false;
		bool bTerrainSelected;
		bool bAgl = false;
		bool startGame = false;
		bool debugMode = false;
		bool hardLanding = false;
		bool correctLanding = false;
		bool gameOver = false;

		int pressCount = 1;
		float keyPressTime;
		float pressedTime;
		float releaseTime;

		float totalFuel = 120 * 1000;
		float score;
		
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;

		vector<Box> bboxList;

		const float selectionRange = 4.0;
		float altitudeLength;

		TreeNode altitude;

		Particle* lunarLander;
		ParticleSystem* sys;
		ThrusterForce* tForce;
		glm::vec3 headingV;
		glm::vec3  normalV;
		ImpluseForce* iForce;
		glm::vec3 landerV;

		// textures
		ofTexture  particleTex;

		// shaders
		ofVbo vbo;
		ofVbo explosionVbo;
		ofShader shader;

		// Trust Emitter and some forces;
		ParticleEmitter exhaustEmitter;

		ImpulseRadialForce* radialForce;
		GravityForce* gravityForce;

		// Emitter and some forces;
		ParticleEmitter explosionEmitter;
		ImpulseRadialForce* explosionRadialForce;

		ofLight keyLight, fillLight, rimLight, landingLight;

		ofSoundPlayer   explosions, movement;
};
