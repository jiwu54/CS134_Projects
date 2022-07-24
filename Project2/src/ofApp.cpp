#include "ofApp.h"

//Implemented based on  Emitter.cpp, Sprite.cpp from Kevin M. Smith - CS 134 SJSU



void Player::draw(bool a) {
	ofSetColor(ofColor::red);
	ofFill();

	ofPushMatrix();
	ofMultMatrix(getMatrix());

	if(a){
		ofSetColor(255,255,255);
		drawSprite();
	}
	else {
		ofDrawTriangle(verts[0], verts[1], verts[2]);
	}
	ofPopMatrix();

	heading();
};

 glm::vec3 Player::heading() {

	 
	float angle = glm::radians(rotation);
	 headingV = glm::normalize(glm::vec3(glm::sin(angle), -glm::cos(angle),
		0));
	ofSetColor(ofColor::green);
	ofDrawLine(pos, pos + headingLength * scales * headingV);
	return headingV;
	
}

 void Player::drawSprite() {
	
		 sprite.load("images/rocket.png");

		 sprite.draw(sprite.getWidth()/32-53, sprite.getHeight()/32-40);
	
 }


 BaseObject::BaseObject() {
	 trans = pos;
	 scale = scales;
	rot = rotation;
 }

 void BaseObject::setPosition(ofVec3f posi) {
	 pos = posi;
	 trans = pos;
 }



 //
// Basic Sprite Object
//
AgentSprite::AgentSprite() {
	//speed = 20;
	//velocity = ofVec3f(0, 0, 0);
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	name = "UnamedSprite";
	width = 60;
	height = 80;
	showAgentSprite = true;
}

// Return a sprite's age in milliseconds
//
float AgentSprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void AgentSprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();
}


void AgentSprite::integrate() {
	// init current framerate (or you can use ofGetLastFrameTime())
//
	float framerate = ofGetFrameRate();
	float dt = 1.0 / framerate;

	// linear motion
	//
	pos += (velocity * dt);
	glm::vec3 accel = acceleration;
	accel += (force * 1.0 / mass);
	velocity += accel * dt;
	velocity *= damping;

	// angular motion
	//
	rotation += (angularVelocity * dt);
	float a = angularAcceleration;;
	a += (angularForce * 1.0 / mass);
	angularVelocity += a * dt;
	angularVelocity *= damping;
}



float AgentSprite::GetOriAngle(glm::vec3 playerPos) {
	float angle = glm::radians(rotation);
	headingV = glm::normalize(glm::vec3(glm::sin(angle), -glm::cos(angle),
		0));
	glm::vec3 v1 = glm::normalize(playerPos - pos);
	oriAngle = glm::orientedAngle(v1, headingV, glm::vec3(0, 0, -1));
	oriAngle += angle;
	float oriAngleInDegree = glm::degrees(oriAngle);
	headingV = glm::normalize(glm::vec3(glm::sin(oriAngle), -glm::cos(oriAngle),
		0));

	return oriAngleInDegree;
}

//  Render the sprite
//
void AgentSprite::draw(bool a, glm::vec3 playerPos) {

	ofSetColor(255, 255, 255, 255);
	showAgentSprite = a;
	// draw image centered and add in translation amount
	//
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	if (a) {
		if (haveImage) {
			image.resize(25 ,25);
			image.draw(sprite.getWidth() / 32 - 10, sprite.getHeight() / 32 - 10);

		}
		else {
			// in case no image is supplied, draw something.
			// 
			ofSetColor(ofColor::yellow);
			ofDrawTriangle(glm::vec3(-10, 10, 0), glm::vec3(10, 10, 0), glm::vec3(0, -10, 0));
		
		}
	}
	else {
		ofDrawTriangle(glm::vec3(-10, 10, 0), glm::vec3(10, 10, 0), glm::vec3(0, -10, 0));
	}
	ofPopMatrix();
	

}

//  Add a Sprite to the Sprite System
//
void AgentSpriteSystem::add(AgentSprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void AgentSpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


// remove all sprites within a given dist of point, return number removed
//
int AgentSpriteSystem::removeNear(ofVec3f point, float dist) {
	vector<AgentSprite>::iterator s = sprites.begin();
	vector<AgentSprite>::iterator tmp;
	int count = 0;

	while (s != sprites.end()) {
		ofVec3f v = s->pos - point;
		if (v.length() < dist) {
			tmp = sprites.erase(s);
			count++;
			s = tmp;
		}
		else s++;
	}
	return count;
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void AgentSpriteSystem::update(glm::vec3 playerPos) {

	if (sprites.size() == 0) return;
	vector<AgentSprite>::iterator s = sprites.begin();
	vector<AgentSprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	

	//  Move sprite
	//
	for (int i = 0; i < sprites.size(); i++) {
		float oriAngleTemp = sprites[i].GetOriAngle(playerPos);
		if (sprites[i].rotation < oriAngleTemp) {
			//rotate sprite using force
			sprites[i].angularForce += 100;
		
		}
		else if (sprites[i].rotation > oriAngleTemp){
			//rotate sprite using force
			sprites[i].angularForce -= 100;
			
		}
		
		//move sprite using force
		sprites[i].force = -glm::vec3(sprites[i].speed * -glm::sin(glm::radians(sprites[i].rotation)), sprites[i].speed * glm::cos(glm::radians(sprites[i].rotation)), 0) * sprites[i].mass;
		
	}

	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].integrate();
	}

	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].force = glm::vec3(0, 0, 0);
		sprites[i].angularForce = 0;
	}

}

//  Render all the sprites
//
void AgentSpriteSystem::draw(bool a, glm::vec3 playerPos) {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw(a, playerPos);
	}
}

//  Create a new Emitter - needs a SpriteSystem
//
AgentEmitter::AgentEmitter(AgentSpriteSystem *spriteSys) {
	sys = spriteSys;
	init();
}

AgentEmitter::AgentEmitter() {
	sys = new AgentSpriteSystem();
	init();
}


void AgentEmitter::init() {
	lifespan = 3000;    // milliseconds
	started = false;

	lastSpawned = 0;
	rate = 1;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = 20;
	drawable = true;
	width = 50;
	height = 50;
	childWidth = 10;
	childHeight = 10;
}



//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void AgentEmitter::draw(bool a, glm::vec3 playerPos) {
	if (drawable) {

		if (haveImage) {
			image.draw(-image.getWidth() / 2.0 + trans.x, -image.getHeight() / 2.0 + trans.y);
		}
		else {
			ofSetColor(0, 0, 200);
			ofDrawRectangle(-width/2 + trans.x, -height/2 + trans.y, width, height);
			

		}
	}

	// draw sprite system
	//
	sys->draw(a, playerPos);
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void AgentEmitter::update(glm::vec3 playerPos) {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();
	if ((time - lastSpawned) > (1000.0/rate)) {
		// spawn a new sprite
		AgentSprite sprite;
		if (haveChildImage) sprite.setImage(childImage);
		sprite.speed = velocity;
		sprite.lifespan = lifespan;
		sprite.setPosition(ofVec3f(ofRandom(ofGetWindowWidth()), ofRandom(ofGetWindowHeight()), 0));
		sprite.birthtime = time;
		sprite.width = childWidth;
		sprite.height = childHeight;

		sys->add(sprite);
		lastSpawned = time;
	}
	sys->update(playerPos);
}


// Start/Stop the emitter.
//
void AgentEmitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void AgentEmitter::stop() {
	started = false;
}


void AgentEmitter::setLifespan(float life) {
	lifespan = life;
}

void AgentEmitter::setVelocity(float v) {
	velocity = v;
}

void AgentEmitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
	childWidth = img.getWidth();
	childHeight = img.getHeight();
}

void AgentEmitter::setImage(ofImage img) {
	image = img;
}

void AgentEmitter::setRate(float r) {
	rate = r;
}






//--------------------------------------------------------------
void ofApp::setup(){

	backgroundImage.load("images/background.png");

	ofSetVerticalSync(true);

	// create an image for sprites being spawned by emitter
	//
	if (defaultImage.load("images/agent.png")) {
		imageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/space-invaders-ship-scaled.png");
		ofExit();
	}


	// set up spacing for row of numEmitters across top of window
	//
	agentSpeeds = 20;
	numEmitters = 5;

	//emit = new Emitter(new SpriteSystem());
	// create an array of emitters and set their position;
	//
	for (int i = 0; i < numEmitters; i++) {
		AgentEmitter* emit = new AgentEmitter(new AgentSpriteSystem());
		emit->setPosition(ofVec3f(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0));
		emit->drawable = false;                // make emitter itself invisible
		emit->setChildImage(defaultImage);
		emit->setVelocity(agentSpeeds);
		emitters.push_back(emit);
	}
	
	explosions.load("sounds/explosion.mp3");
	collisions.load("sounds/collision.mp3");
	movement.load("sounds/movement.mp3");
	movement.setVolume(0.1);
	collisions.setVolume(0.5);
	explosions.setVolume(0.5);

	gui.setup();

	gui.add(playerScale.setup("Player Scale", 0.5, 0.5, 1));
	gui.add(speed.setup("Player Speed", 300, 100, 600));
	gui.add(showSprite.setup("Player Show Sprite", false));
	gui.add(showHeading.setup("Player Show Heading", true));
	gui.add(agentShowSprite.setup("Agent Show Sprite", false));
	gui.add(energyLevel.setup("EnergyLevel", 15, 0, 15));
	gui.add(numAgents.setup("Number Of Agent", 1, 1, 5));
	gui.add(rate.setup("Agent Spawn Rate", 1, .5, 10));
	gui.add(life.setup("Agent Life", 5, 0, 10));
	gui.add(agentSpeeds.setup("Agent Speed", 30, 10, 100));
	gui.add(gameLevels.setup("Difficulty Levels", 0, 0, 3));

	ply = Player(glm::vec3(-50, 50, 0), glm::vec3(50, 50, 0), glm::vec3(0, -50, 0));
	ply.pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	ply.rotation = 0;
	ply.scales = glm::vec3(playerScale);
	bHide = false;

	numAgents = 1;

	explosionsEmitter.setEmitterType(RadialEmitter);
	explosionsEmitter.setParticleRadius(2);
	explosionsEmitter.setOneShot(true);
	explosionsEmitter.setGroupSize(30);
	

	particleRay.setEmitterType(DirectionalEmitter);
	particleRay.setParticleRadius(7);
	particleRay.setOneShot(true);
	particleRay.setGroupSize(1);
	
	
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSeedRandom();
	checkBound();

	explosionsEmitter.setVelocity(glm::vec3(50, 50, 0));
	explosionsEmitter.setLifespan(2);

	particleRay.setPosition(ply.pos);
	particleRay.setVelocity(ply.heading()*300);
	particleRay.setLifespan(7);
	


	backgroundImage.resize(ofGetWindowWidth(), ofGetWindowHeight());


	energyLevels = energyLevel;

	checkCollisions();

	if (ply.pos.y == 0) {
		ply.pos.y++;
	}

	if (gameLevels == 3) {
		agentSpeeds = 70;
		numAgents = 5;
		rate = 5;
		life = 4;

	}
	else if (gameLevels == 2) {
		agentSpeeds = 40;
		numAgents = 3;
		rate = 4;
		life = 4;
	}
	else if (gameLevels == 1) {
		agentSpeeds = 30;
		numAgents = 2;
		rate = 3;
		life = 5;
		
	} 



	for (int i = 0; i < emitters.size(); i++) {
		AgentEmitter* emitter = emitters[i];
		emitter->setRate(rate);
		emitter->setLifespan(life * 1000);    // convert to milliseconds 
		emitter->setVelocity(agentSpeeds);
		emitter->update(ply.pos);
	
	}



	if (dragged == true && mousep == true) {
		ply.pos = glm::vec3(mouseX, mouseY, 0) + delta;
	};
	ply.scales = glm::vec3(playerScale);


	if (showHeading) {
		ply.headingLength = 75;
	}
	else {
		ply.headingLength = 0;
	}

	if (showSprite) {
		ply.draw(showSprite);
	}
	else {
		ply.draw(showSprite);
	}
	
	if (gamestared) {
		for (int i = numAgents; i < emitters.size(); i++) {
			for (int j = 0; j < emitters[i]->sys->sprites.size(); j++){

				emitters[i]->sys->sprites[j].scales = glm::vec3(0, 0, 0);
				emitters[i]->sys->sprites[j].lifespan = 0.0;
		}
			
		}

		time = ofGetElapsedTimeMillis() / 1000;
		totalgameTime = (time - gameStartTime);
	}


	if (energyLevel == 0) {
		gameOver = true;
		for (int i = 0; i < emitters.size(); i++) {
			emitters[i]->stop();
		}

		
	}

	explosionsEmitter.update();
	particleRay.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(255, 255, 255);
	backgroundImage.draw(0, 0);

	string frameRateText;
	frameRateText += "FPS: " + std::to_string(int(ofGetFrameRate()));
	ofDrawBitmapString(frameRateText, ofPoint(ofGetWindowWidth() - 80, 40));

	string instrText;
	instrText += "Press <SpaceBar> to Start/Restart Game";
	ofDrawBitmapString(instrText, ofPoint(ofGetWindowWidth() - 325, 20));

	string eneText;
	eneText += "Energy Level: " + std::to_string(energyLevel);
	ofDrawBitmapString(eneText, ofPoint(ofGetWindowWidth() - 150, 60));

	
	ofDrawBitmapString("Elapsed Time: " + std::to_string(int(totalgameTime)) + "s", ofPoint(ofGetWindowWidth() - 150, 80));

	string fullScreenText;
	fullScreenText += "Press <f> to Enter/Exit fullscreen";
	ofDrawBitmapString(fullScreenText, ofPoint(ofGetWindowWidth() - 295, 100));


	string hideGuiText;
	hideGuiText += "Press <h> to Show/Hide GUI";
	ofDrawBitmapString(hideGuiText, ofPoint(ofGetWindowWidth() - 230, 120));

	string fireText;
	fireText += "Press <d> to Fire";
	ofDrawBitmapString(fireText, ofPoint(ofGetWindowWidth() - 230, 140));

	string switchText;
	switchText += "Press <t> to Switch theme";
	ofDrawBitmapString(switchText, ofPoint(ofGetWindowWidth() - 230, 160));

	string currentThemeText,state;
	state = (shootingTheme ? "ON" : "OFF");
	currentThemeText += "Shooting Theme : " + state;
	ofDrawBitmapString(currentThemeText, ofPoint(ofGetWindowWidth() - 230, 180));

	string levelText;
	levelText += "Levels Available: \n 0 (Silders Adjustable)\n 1 (Easy)\n 2 (Medium)\n 3 (Hard)";
	ofDrawBitmapString(levelText, ofPoint(0+10, ofGetWindowHeight()/2));


	if (gameOver) {
		ofDrawBitmapString("GAME OVER", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
		ofDrawBitmapString("Total Game Time: " + std::to_string(int(totalgameTime)) + "s", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2 + 20));
	}
	else {
	for (int i = 0; i < emitters.size(); i++) {
			emitters[i]->draw(agentShowSprite,ply.pos);
		}

		ply.draw(showSprite);

		explosionsEmitter.draw();
		particleRay.draw();

		if (!bHide) {
			gui.draw();

		}
	}
	

	


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	keymap[key] = true;

	if (keymap[OF_KEY_RIGHT]) {

		ply.rotation += 3;
		movement.play();
		
	}
	if (keymap[OF_KEY_LEFT]) {
		ply.rotation -= 3;
		movement.play();
	}
	if (keymap[OF_KEY_UP]) {


		if (!checkBound()) {
			ply.pos += (speed / ofGetFrameRate()) * ply.heading();
		}
		else {
			ply.pos -= (speed / ofGetFrameRate()) * ply.heading();
			ply.pos -= (speed / ofGetFrameRate()) * ply.heading();
		
		}
		movement.play();
	}
	if(keymap[OF_KEY_DOWN]) {
	
		if (!checkBound()) {
			ply.pos -= (speed / ofGetFrameRate()) * ply.heading();
		}
		else {
			ply.pos += (speed / ofGetFrameRate()) * ply.heading();
			ply.pos += (speed / ofGetFrameRate()) * ply.heading();
			
		}
		movement.play();
	}
	switch (key) {
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case ' ':
		gameStartTime = ofGetElapsedTimeMillis() / 1000;
		ply.pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
		for (int i = 0; i < emitters.size(); i++) {
			if (!emitters[i]->started) {
				emitters[i]->start();
			}
		}

		gamestared = true;
		gameOver = false;

		energyLevel = 15;
		break;

	case 'D':
	case 'd':
		if (shootingTheme) {
			particleRay.sys->reset();
			particleRay.start();
		}
		break;

	case 'T':
	case 't':
		shootingTheme = !shootingTheme;
		break;
	}




}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	keymap[key] = false;
	

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (dragged == true && mousep == true) {
		mouseX = x;
		mouseY = y;
	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	mousep = true;
	if (ply.inside(glm::vec3(x, y, 0))) {
		dragged = true;
		delta = ply.pos - glm::vec3(x, y, 0);
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	mousep = false;
	dragged = false;
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
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


void ofApp::checkCollisions() {

	// find the distance at which the two sprites (missles and invaders) will collide
	// detect a collision when we are within that distance.
	//
	for (int j = 0; j < emitters.size(); j++) {
		float collisionDist = (emitters[j]->childHeight / 2)+10;

		// Loop through all the missiles, then remove any invaders that are within
		// "collisionDist" of the missiles.  the removeNear() function returns the
		// number of missiles removed.
		//

		for (int i = 0; i < emitters[j]->sys->sprites.size(); i++) {
			int prevEnergyLevel = energyLevels;
			
			energyLevels -= emitters[j]->sys->removeNear(ply.pos, collisionDist);
			if (energyLevels < prevEnergyLevel) {
				explosionsEmitter.setPosition(ply.pos);
				explosionsEmitter.sys->reset();
				explosionsEmitter.start();
				collisions.play();
			}

			if (energyLevels < 0) {
				energyLevels = 0;
			}
			energyLevel = energyLevels;
			
		}
	}

	if (shootingTheme) {
		for (int j = 0; j < emitters.size(); j++) {
			float collisionDist = (emitters[j]->childHeight / 2);

			// Loop through all the missiles, then remove any invaders that are within
			// "collisionDist" of the missiles.  the removeNear() function returns the
			// number of missiles removed.
			//

			for (int i = 0; i < emitters[j]->sys->sprites.size(); i++) {
				for (int k = 0; k < particleRay.sys->particles.size(); k++) {
					int prevEnergyLevel = energyLevels;

					energyLevels += emitters[j]->sys->removeNear(particleRay.sys->particles[k].position, collisionDist);
					if (energyLevels > prevEnergyLevel) {
						explosionsEmitter.setPosition(particleRay.sys->particles[k].position);
						explosionsEmitter.sys->reset();
						explosionsEmitter.start();
						explosions.play();
					}

					if (energyLevels < 0) {
						energyLevels = 0;
					}
					energyLevel = energyLevels;

				}

			}
		}
	}

}


bool ofApp::checkBound() {

	for (int i = 0; i < ply.verts.size(); i++) {
		glm::vec3 v1 = ply.getMatrix() * glm::vec4(ply.verts[i], 1);
		if (v1.x >= ofGetWindowWidth()) {
			float temp = v1.x - ofGetWindowWidth();
			ply.pos.x = ply.pos.x - temp;
			return true;
		}
		if (v1.x <= 0) {
			float temp = 0 - v1.x;
			ply.pos.x = ply.pos.x + temp;
		
			return true;
		}
		if (v1.y >= ofGetWindowHeight()) {
			float temp = v1.y - ofGetWindowHeight();
			ply.pos.y = ply.pos.y - temp;
			
			return true;
		}
		if (v1.y <= 0) {
			float temp = 0 - v1.y;
			ply.pos.y = ply.pos.y + temp;
			
			return true;
		}
	}

	return false;


	//}

}