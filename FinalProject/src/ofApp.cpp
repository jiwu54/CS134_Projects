
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  CS134-Final-Project-Spring-2022
// 
//
//  Student Name:   < JianBin Wu >
//  Date: <05/13/2022>


#include "ofApp.h"
#include "Util.h"
#include <glm/gtx/intersect.hpp>


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	ofSetFrameRate(60);
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bTerrainSelected = true;

	//sound effect
	backgroundImage.load("images/background.png");
	explosions.load("sounds/explosion.mp3");
	movement.load("sounds/movement.mp3");
	movement.setVolume(0.3);
	movement.setLoop(true);
	explosions.setVolume(0.1);
	
	//cam setup
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	cam.setGlobalPosition(glm::vec3(0, 50, 75));
	cam1.setNearClip(.1);
	cam1.setFov(65.5);
	cam1.rotateDeg(10, glm::vec3(1, 0, 0));
	cam2.setNearClip(.1);
	cam2.setFov(65.5);
	cam3.setNearClip(.1);
	cam3.setFov(65.5);
	cam3.setGlobalPosition(glm::vec3(30.6382, 5, 30.6406));
	cam4.setNearClip(.1);
	cam4.setFov(65.5);
	cam4.rotateDeg(45, glm::vec3(1, 0, 0));
	theCam = &cam;

	ofEnableSmoothing();
	ofEnableDepthTest();
	ofEnableLighting();
	
	//light setup
	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(1, 1);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.setSpecularColor(ofFloatColor(1, 1, 1));
	keyLight.rotate(45, ofVec3f(0, 1, 0));
	keyLight.rotate(-45, ofVec3f(1, 0, 0));
	keyLight.setPosition(200, 80, 0);

	fillLight.setup();
	fillLight.enable();
	fillLight.setAreaLight(1, 1);
	fillLight.setScale(.05);
	fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
	fillLight.rotate(45, ofVec3f(0, 1, 0));
	fillLight.rotate(-45, ofVec3f(1, 0, 0));
	fillLight.setPosition(-200, 50, 200);

	rimLight.setup();
	rimLight.enable();
	rimLight.setAreaLight(1, 1);
	rimLight.setScale(.05);
	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
	rimLight.rotate(45, ofVec3f(0, 1, 0));
	rimLight.rotate(-45, ofVec3f(1, 0, 0));
	rimLight.setPosition(-200, 30, -200);

	// texture loading
	//
	ofDisableArbTex();     // disable rectangular textures

	// load textures
	//
	if (!ofLoadImage(particleTex, "images/dot.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}

	// load the shader
	//
#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
#endif

	
	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	moon.loadModel("geo/moon-houdini.obj");
	moon.setScaleNormalization(false);

	// create sliders for testing
	//
	gui.setup();
	gui.add(gravity.setup("Gravity", 1.64, -20, 20));
	gui.add(showHeading.setup("Show Heading", false));


	//  Create Octree for testing.
	//
	octree.create(moon.getMesh(0), 20);

	cout << "Number of Verts: " << moon.getMesh(0).getNumVertices() << endl;

	landingArea = Box(Vector3(-5, 0, -5), Vector3(5, 0.3, 5));

	lander.loadModel("geo/lander.obj");
	lander.setScaleNormalization(false);
	bLanderLoaded = true;

	gravityForce = new GravityForce(ofVec3f(0, -gravity, 0));

	//lander setup
	lunarLander = new Particle();
	sys = new ParticleSystem();
	lander.setPosition(-150, 85, -150);
	lunarLander->lifespan = -1;
	tForce = new ThrusterForce(ofVec3f(0, 0, 0));
	
	sys->add(*lunarLander);
	sys->addForce(gravityForce);
	sys->addForce(tForce);
	//sys->addForce(new TurbulenceForce(ofVec3f(-1, -1, -1), ofVec3f(1, 1, 1)));

	// Exhaust Emitter setup
	exhaustEmitter.setEmitterType(DiscEmitter);
	exhaustEmitter.setPosition(sys->particles[0].position);
	exhaustEmitter.setVelocity(ofVec3f(0, -10, 0));
	exhaustEmitter.setOneShot(true);
	exhaustEmitter.setGroupSize(50);
	exhaustEmitter.setLifespan(.50);
	exhaustEmitter.sys->addForce(new TurbulenceForce(ofVec3f(-50, -50, -50), ofVec3f(50,50, 50)));
	exhaustEmitter.sys->addForce(new CyclicForce(10));
	exhaustEmitter.sys->addForce(gravityForce);
	radialForce = new ImpulseRadialForce(100);
	radialForce->setHeight(0.19);
	exhaustEmitter.sys->addForce(radialForce);
	exhaustEmitter.setMass(5);


	// Explosion Emitter Setup
	explosionRadialForce = new ImpulseRadialForce(1000);
	explosionEmitter.sys->addForce(new TurbulenceForce(ofVec3f(-4, -4, -4), ofVec3f(4, 4, 4)));
	explosionEmitter.sys->addForce(gravityForce);
	explosionEmitter.sys->addForce(explosionRadialForce);
	explosionEmitter.sys->addForce(new CyclicForce(5));
	radialForce->setHeight(0.1);
	explosionEmitter.setVelocity(ofVec3f(0, 0, 0));
	explosionEmitter.setOneShot(true);
	explosionEmitter.setEmitterType(RadialEmitter);
	explosionEmitter.setGroupSize(100);
	explosionEmitter.setRandomLife(true);
	explosionEmitter.setLifespanRange(ofVec2f(1, 3));

}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {

	ofSeedRandom();
	gravityForce->set(ofVec3f(0, -gravity, 0));
	if (bLanderLoaded && !bLanderSelected) {
		if (totalFuel <= 0) {
			totalFuel = 0;
			fuel = false;
		}
		sys->particles[0].position = lander.getPosition();
		if (checkCollisions()) {
			landed = true;
			if (landed) {
				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				if (bounds.overlap(landingArea)) {
					correctLanding = true;
					score = (totalFuel / 1000) + 50;
				}
			}

			//collision respond
			if (sys->particles[0].velocity.y < -5 || !correctLanding) {
				if (!gameOver) {
					landerV = glm::vec3(sys->particles[0].velocity.x, sys->particles[0].velocity.y, sys->particles[0].velocity.z);
					normalV = glm::vec3(0, 60, 0);
					iForce = new ImpluseForce(((1 + 0.55) * ((glm::dot(-landerV, normalV)) * normalV) * (1 / ofGetFrameRate())));
					iForce->applyOnce = true;
					sys->addForce(iForce);
					explosionEmitter.sys->reset();
					explosionEmitter.start();
					explosions.play();
				}
				gameOver = true;
			}
			else if (sys->particles[0].velocity.y < -3 && correctLanding) {
				hardLanding = true;
				score = (totalFuel / 1000) + 25;
			}
			
		}
		
		sys->update();
		lander.setPosition(sys->particles[0].position.x, sys->particles[0].position.y, sys->particles[0].position.z);
		lander.setRotation(0, sys->particles[0].rotation, 0, 1, 0);
		
	
		aglCalculation();

		exhaustEmitter.setPosition(sys->particles[0].position);
		exhaustEmitter.update();
		explosionEmitter.setPosition(sys->particles[0].position + ofVec3f(0,3,0));
		explosionEmitter.update();
	}

	cam1.setPosition(glm::vec3(lander.getPosition().x, lander.getPosition().y + 0.3, lander.getPosition().z));
	cam2.setGlobalPosition(glm::vec3(lander.getPosition().x, lander.getPosition().y -20, lander.getPosition().z));
	cam2.lookAt(glm::vec3(lander.getPosition().x, 1, lander.getPosition().z));
	cam3.lookAt(lander.getPosition());
	cam4.setPosition(lander.getPosition().x, lander.getPosition().y + 25, lander.getPosition().z - 25);
	cam4.lookAt(lander.getPosition());
}
//--------------------------------------------------------------
void ofApp::draw() {
	
	loadVbo();
	loadExplosionVbo();

	glDepthMask(false);
	ofSetColor(255, 255, 255);
	backgroundImage.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	if (!bHide) gui.draw();
	glDepthMask(true);

	theCam->begin();
	ofPushMatrix();

		ofEnableLighting();  // shaded mode
		moon.drawFaces();
		ofMesh mesh;
		ofNoFill();
		ofSetColor(ofColor::green);
		Octree::drawBox(landingArea);
		ofFill();
		if (bLanderLoaded) {
			lander.drawFaces();
			exhaustEmitter.draw();
			float angle = glm::radians(sys->particles[0].rotation);
			headingV = glm::normalize(glm::vec3(-glm::sin(angle), 0,
				-glm::cos(angle)));
			if (showHeading) {
				ofSetColor(ofColor::red);
				ofDrawLine(lander.getPosition(), lander.getPosition() + (75 * headingV));
			}
			
			if (bAgl) {
				ofSetColor(ofColor::green);
				ofDrawLine(lander.getPosition(), octree.mesh.getVertex(altitude.points[0]));
			}
			

			if (!bTerrainSelected) drawAxis(lander.getPosition());

			if (bLanderSelected) {

				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofNoFill();
				ofSetColor(ofColor::white);
				Octree::drawBox(bounds);

				// draw colliding boxes
				//
				ofSetColor(ofColor::lightBlue);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));

	ofDisableLighting();

	ofPopMatrix();
	theCam->end();

	glDepthMask(GL_FALSE);

	ofSetColor(ofColor::yellow);

	// this makes everything look glowy :)
	//
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();


	// begin drawing  shader in the camera
//
	shader.begin();
	theCam->begin();

	// draw particle emitter here..
	//
	particleTex.bind();
	vbo.draw(GL_POINTS, 0, (int)exhaustEmitter.sys->particles.size());
	explosionVbo.draw(GL_POINTS, 0, (int)explosionEmitter.sys->particles.size());
	particleTex.unbind();

	//  end drawing in the camera
	// 
	theCam->end();
	shader.end();

	ofDisablePointSprites();
	ofDisableBlendMode();
	ofEnableAlphaBlending();

	// set back the depth mask
	//
	glDepthMask(GL_TRUE);

	string frameRateText;
	frameRateText += "FPS: " + std::to_string(int(ofGetFrameRate()));
	ofDrawBitmapString(frameRateText, ofPoint(ofGetWindowWidth() - 80, 40));

	string instrText;
	instrText += "<SpaceBar> Start/Restart Game";
	ofDrawBitmapString(instrText, ofPoint(ofGetWindowWidth() - 250, 20));

	string aglText;
	aglText += "AGL<a>: " + std::to_string(altitudeLength);
	ofDrawBitmapString(aglText, ofPoint(ofGetWindowWidth()/2, 40));

	string fullScreenText;
	fullScreenText += "<f> Enter/Exit fullscreen";
	ofDrawBitmapString(fullScreenText, ofPoint(ofGetWindowWidth() - 220, 60));

	string hideGuiText;
	hideGuiText += "<h> Debug Mode/Show GUI";
	ofDrawBitmapString(hideGuiText, ofPoint(ofGetWindowWidth() - 210, 80));

	
	string fuelText;
	fuelText += "Fuel: " + std::to_string(totalFuel / 1000) + "/120 (s)";
	ofDrawBitmapString(fuelText, ofPoint(ofGetWindowWidth() / 2, 20));

	string camText;
	camText += "<F1> - FreeCam, <F2 - F4> Other Cams";
	ofDrawBitmapString(camText, ofPoint(ofGetWindowWidth() - 310, 100));

	string camInstrText;
	camInstrText += "<z/x> Rotate Onboard Cam(F2), <r> Reset Freecam";
	ofDrawBitmapString(camInstrText, ofPoint(ofGetWindowWidth() - 400, 120));

	string moveText;
	moveText += "<Arrow Keys/w/s/q/e> Ship Movement";
	ofDrawBitmapString(moveText, ofPoint(ofGetWindowWidth() / 2, 60));

	if (gameOver) {
		ofDrawBitmapString("GAME OVER!", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		ofDrawBitmapString("Landed On Wrong Area / Ship Exploded", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 +20));
	}

	if (correctLanding && !gameOver) {
		ofDrawBitmapString("Correct Landing(+50)", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		ofDrawBitmapString("Total Socre: " + std::to_string(int(score)) + " (Landing Score + Fuel Left)", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 +20));
	}
	if (hardLanding && !gameOver) {
		ofDrawBitmapString("Hard Landing(+25)", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		ofDrawBitmapString("Total Socre: " + std::to_string(int(score)) + " (Landing Score + Fuel Left)", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 +20));
	}
	
}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case ' ': //start game / restart
		gameOver = false;
		correctLanding = false;
		hardLanding = false;
		if (!debugMode) {
			lander.setPosition(-150,85,-150);
			theCam = &cam4;
			gravity = 1.64;
		}
		if (debugMode){theCam = &cam;}
		bLanderSelected = false;
		totalFuel = 120 * 1000;
		pressCount = 1;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		debugMode = !debugMode;
		break;
	case 'A':
	case 'a':
		bAgl = !bAgl; //AGL Sensor
		break;
	case 'r':
		cam.reset();
		cam.setPosition(lander.getPosition().x, lander.getPosition().y + 25,lander.getPosition().z + 25);
		cam.rotateDeg(-45,glm::vec3(1,0,0));
		break;
	case 't':
		setCameraTarget();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;	
	case 'W':
	case 'w': //move upward
		if (fuel && !gameOver) {
		tForce->add(ofVec3f(0, 1, 0));
				exhaustEmitter.start();
				if (!movement.isPlaying()) { 
					movement.play();
				}
				if (pressCount == 1) {
					pressedTime = ofGetElapsedTimeMillis();
					pressCount--;
				}
		}
		break;
	case 'S':
	case 's': //move downward
		if (fuel && !gameOver) {
			tForce->add(ofVec3f(0, -1, 0));
			exhaustEmitter.start();
			if (!movement.isPlaying()) {
				movement.play();
			}
			if (pressCount == 1) {
				pressedTime = ofGetElapsedTimeMillis();
				pressCount--;
			}
		}
		break;
	case 'Q':
	case 'q':
		if (fuel && !gameOver) {
			sys->particles[0].angularForce = 50; //rotate left
			exhaustEmitter.start();
			if (!movement.isPlaying()) {
				movement.play();
			}
			if (pressCount == 1) {
				pressedTime = ofGetElapsedTimeMillis();
				pressCount--;
			}
		}
		break;
	case 'E':
	case 'e':
		if (fuel && !gameOver) {
			sys->particles[0].angularForce = -50; //rotate right
			exhaustEmitter.start();
			if (!movement.isPlaying()) {
				movement.play();
			}
			if (pressCount == 1) {
				pressedTime = ofGetElapsedTimeMillis();
				pressCount--;
			}
		}
		break;
	case OF_KEY_LEFT:   // move left (along x axis)
		if (fuel && !gameOver) {
			tForce->add(ofVec3f(-1, 0, 0));
			exhaustEmitter.start();
			if (!movement.isPlaying()) {
				movement.play();
			}
			if (pressCount == 1) {
				pressedTime = ofGetElapsedTimeMillis();
				pressCount--;
			}
		}
		break;
	case OF_KEY_RIGHT:  // move right (along x axis)
		if (fuel && !gameOver) {
			tForce->add(ofVec3f(1, 0, 0));
			exhaustEmitter.start();
			if (!movement.isPlaying()) {
				movement.play();
			}
			if (pressCount == 1) {
				pressedTime = ofGetElapsedTimeMillis();
				pressCount--;
			}
		}
		break;
	case OF_KEY_UP:     // go forward
		if (fuel && !gameOver) {
			tForce->add(1 * headingV);
			exhaustEmitter.start();
			if (!movement.isPlaying()) {
				movement.play();
			}
			if (pressCount == 1) {
				pressedTime = ofGetElapsedTimeMillis();
				pressCount--;
			}
		}
		break;
	case OF_KEY_DOWN:   // go backward
		if (fuel && !gameOver) {
			tForce->add(-1 * headingV);
			exhaustEmitter.start();
			if (!movement.isPlaying()) {
				movement.play();
			}
			if (pressCount == 1) {
				pressedTime = ofGetElapsedTimeMillis();
				pressCount--;
			}
		}
		break;
	case OF_KEY_F1:
		theCam = &cam; //freecam
		break;
	case OF_KEY_F2:
		theCam = &cam1; //on board cam
		break;
	case OF_KEY_F3:
		theCam = &cam2; //bottom cam
		break;
	case OF_KEY_F4:
		theCam = &cam3; //tracking cam
		break;
	case OF_KEY_F5:
		theCam = &cam4; //tracking cam2
		break;
	case 'Z':
	case 'z':
		theCam->rotateDeg(1, glm::vec3(0, 1, 0));
		break;
	case 'X':
	case 'x':
		theCam->rotateDeg(-1, glm::vec3(0, 1, 0));
		break;
	default:
		break;
	}
}



void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}


void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case 'W':
	case 'w':
		if (fuel && !gameOver) {
			tForce->set(ofVec3f(0, 0, 0));
			exhaustEmitter.stop();
			movement.stop();
			releaseTime = ofGetElapsedTimeMillis();
			keyPressTime = releaseTime - pressedTime;
			totalFuel = totalFuel - keyPressTime;
			pressCount++; //makesure pressedTime won't overwrite

		}
		break;
	case 'S':
	case 's':
		if (fuel && !gameOver) {
			tForce->set(ofVec3f(0, 0, 0));
			exhaustEmitter.stop();
			movement.stop();
			releaseTime = ofGetElapsedTimeMillis();
			keyPressTime = releaseTime - pressedTime;
			totalFuel = totalFuel - keyPressTime;
			pressCount++;
		}
		break;
	case 'Q':
	case 'q':
		if (fuel && !gameOver) {
			sys->particles[0].angularForce = 0;
			exhaustEmitter.stop();
			movement.stop();
			releaseTime = ofGetElapsedTimeMillis();
			keyPressTime = releaseTime - pressedTime;
			totalFuel = totalFuel - keyPressTime;
			pressCount++;
		}

		break;
	case 'E':
	case 'e':
		if (fuel && !gameOver) {
			sys->particles[0].angularForce = 0;
			exhaustEmitter.stop();
			movement.stop();
			releaseTime = ofGetElapsedTimeMillis();
			keyPressTime = releaseTime - pressedTime;
			totalFuel = totalFuel - keyPressTime;
			pressCount++;
		}
		break;
	case OF_KEY_LEFT: 
		if (fuel && !gameOver) {
			tForce->set(ofVec3f(0, 0, 0));
			exhaustEmitter.stop();
			movement.stop();
			releaseTime = ofGetElapsedTimeMillis();
			keyPressTime = releaseTime - pressedTime;
			totalFuel = totalFuel - keyPressTime;
			pressCount++;
		}
		break;
	case OF_KEY_RIGHT:
		if (fuel && !gameOver) {
			tForce->set(ofVec3f(0, 0, 0));
			exhaustEmitter.stop();
			movement.stop();
			releaseTime = ofGetElapsedTimeMillis();
			keyPressTime = releaseTime - pressedTime;
			totalFuel = totalFuel - keyPressTime;
			pressCount++;
		}
		break;
	case OF_KEY_UP:
		if (fuel && !gameOver) {
			tForce->set(ofVec3f(0, 0, 0));
			exhaustEmitter.stop();
			movement.stop();
			releaseTime = ofGetElapsedTimeMillis();
			keyPressTime = releaseTime - pressedTime;
			totalFuel = totalFuel - keyPressTime;
			pressCount++;
		}
		break;
	case OF_KEY_DOWN:
		if (fuel&& !gameOver) {
			tForce->set(ofVec3f(0, 0, 0));
			exhaustEmitter.stop();
			movement.stop();
			releaseTime = ofGetElapsedTimeMillis();
			keyPressTime = releaseTime - pressedTime;
			totalFuel = totalFuel - keyPressTime;
			pressCount++;
		}
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	pointSelected = octree.intersect(ray, octree.root, selectedNode);

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);

	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
	if (lander.loadModel(dragInfo.files[0])) {
		lander.setScaleNormalization(false);
//		lander.setScale(.1, .1, .1);
	//	lander.setPosition(point.x, point.y, point.z);
		lander.setPosition(1, 1, 0);

		bLanderLoaded = true;
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		cout << "Mesh Count: " << lander.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (lander.loadModel(dragInfo.files[0])) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		//		lander.setRotation(1, 180, 1, 0, 0);

				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = cam.getPosition();
		glm::vec3 camAxis = cam.getZAxis();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = lander.getSceneMin();
			glm::vec3 max = lander.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for lander while we are at it
			//
			landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
	}


}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}

// load vertex buffer in preparation for rendering
//
void ofApp::loadVbo() {
	if (exhaustEmitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < exhaustEmitter.sys->particles.size(); i++) {
		points.push_back(exhaustEmitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(25));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

// load vertex buffer in preparation for rendering
//
void ofApp::loadExplosionVbo() {
	if (explosionEmitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < explosionEmitter.sys->particles.size(); i++) {
		points.push_back(explosionEmitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(20));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	explosionVbo.clear();
	explosionVbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	explosionVbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

//AGL
void ofApp::aglCalculation() {
	Ray altitudeRay = Ray(Vector3(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z), Vector3(0, -1, 0));
	if (octree.intersect(altitudeRay, octree.root, altitude)) {
		for (int i = 0; i < altitude.points.size(); i++) {
			altitudeLength = glm::length(octree.mesh.getVertex(altitude.points[i]) - lander.getPosition());
		}
	}
}

//collision detectiom
bool ofApp::checkCollisions() {
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();

	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	colBoxList.clear();
	
	octree.intersect(bounds, octree.root, colBoxList);
		
	for (int i = 0; i < colBoxList.size(); i++) {
		if (bounds.overlap(colBoxList[i])) {
			ofVec3f temps = ofVec3f(colBoxList[i].parameters[0].x(), colBoxList[i].parameters[1].y(), colBoxList[i].parameters[0].z());
			float temp = sys->particles[0].position.y - temps.y;
			sys->particles[0].position.y = sys->particles[0].position.y - (temp - 0.03);
			return true;
		}
	}
	return false;
}
