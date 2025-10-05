//
//  SinglePassFlowFieldShaders.cpp
//  TimeOfTideFlowProjection
//
//  Created by James Kong on 6/10/2025.
//

#include "SinglePassFlowFieldShaders.hpp"

void SinglePassFlowFieldShaders::setup() {
	enableWatchFiles();
	densityWidth = ofGetWidth();
	densityHeight = ofGetHeight();
	simulationWidth = densityWidth / 2;
	simulationHeight = densityHeight / 2;
	windowWidth = ofGetWindowWidth();
	windowHeight = ofGetWindowHeight();

	fileChangedTimes.clear();

	parametersA.setName("buffer A");
	// Shader GUI parameters (defaults match original shader constants)

	parametersB.setName("buffer B");
	
	parametersC.setName("buffer C");

	parametersD.setName("buffer D");

	parametersImage.setName("image");
 
	parameters.setName("SinglePassFlowFieldShaders");
	parameters.add(parametersA);
	parameters.add(parametersB);
	parameters.add(parametersC);
	parameters.add(parametersD);
	parameters.add(parametersImage);
	positionsGroup.setName("positions");
	for (int i = 0; i < MAX_POS; i++) {
		positionsGroup.add(positionsParameter[i]
						   .set(
								"position" + ofToString(i)
								// random init position
								,
								ofVec3f(0,
										ofRandom(ofGetHeight()),
										0)
								,
								ofVec3f::zero()
								,
								ofVec3f(ofGetWidth(), ofGetHeight(), 1)));
	}
	parameters.add(positionsGroup);

}

bool SinglePassFlowFieldShaders::load(string shaderfilename, Buffer buffer) {

	ofFile common;
	common.open("shaders/common.glsl");
	string c = common.readToBuffer().getText();
	ofFile f;
	f.open(shaderfilename);
	ofBuffer b = f.readToBuffer();
	bool result = false;

	switch (buffer) {
	case Buffer::BufferA:
		fileA = f;
		fileAFileName = shaderfilename;
		fileChangedTimes.push_back(getLastModified(fileA));
		result = makeBufferA(c + "\n" + b.getText(), fboA);
	case Buffer::BufferB:
		fileB = f;
		fileBFileName = shaderfilename;
		fileChangedTimes.push_back(getLastModified(fileB));
		result = makeBufferB(c + "\n" + b.getText(), fboB);
	case Buffer::BufferC:
		fileC = f;
		fileCFileName = shaderfilename;
		fileChangedTimes.push_back(getLastModified(fileC));
		result = makeBufferC(c + "\n" + b.getText(), fboC);
	case Buffer::BufferD:
		fileD = f;
		fileDFileName = shaderfilename;
		fileChangedTimes.push_back(getLastModified(fileD));
		result = makeBufferD(c + "\n" + b.getText(), fboD);
	case Buffer::Image:
	default:
		fileImage = f;
		fileImageFileName = shaderfilename;
		fileChangedTimes.push_back(getLastModified(fileImage));
		result = makeShader(c + "\n" + b.getText(), mainShader);
	}
	return result;
}

void SinglePassFlowFieldShaders::applyTexture(ofxShadertoy::Buffer buffer) {
	switch (buffer) {
	case Buffer::BufferA:
			setTexture(0, fboA, buffer);
		return;
	case Buffer::BufferB:
//		setTexture(0, fboA, buffer);
		//			setTexture(1, fboB, buffer);
//		setTexture(2, fboC, buffer);
		//			setTexture(3, fboD, buffer);
	case Buffer::BufferC:
		//			setTexture(0, fboA, buffer);
//		setTexture(1, fboB, buffer);
		//			setTexture(2, fboC, buffer);
		//			setTexture(3, fboD, buffer);
	case Buffer::BufferD:
		//			setTexture(0, fboA, buffer);
		//			setTexture(1, fboB, buffer);
//		setTexture(2, fboC, buffer);
//		setTexture(3, fboD, buffer);
	case Buffer::Image:
	default:
		setTexture(0, fboA, buffer);
//		setTexture(1, fboB, buffer);
//		setTexture(2, fboC, buffer);
//		setTexture(3, fboD, buffer);
	}
}

void SinglePassFlowFieldShaders::setupAdditionalShaderUniforms(const ofShader & bufShader, Buffer buffer) {
	frame += 1 ;
	float dt = 1.0 / max(ofGetFrameRate(), 1.f);
	switch (buffer) {
	case ofxShadertoy::Buffer::BufferA:
		bufShader.setUniforms(parametersA);
		bufShader.setUniform3fv("positions", &positions[0].x, MAX_POS);
		bufShader.setUniform1f("iTimeDelta", dt);
		break;
//	case ofxShadertoy::Buffer::BufferB:
//		
//		bufShader.setUniforms(parametersB);
//			break;
//		case ofxShadertoy::Buffer::BufferC:
//		bufShader.setUniforms(parametersC);
//		break;
//	case ofxShadertoy::Buffer::BufferD:
//		bufShader.setUniforms(parametersD);
//		break;
	case ofxShadertoy::Buffer::Image:
		bufShader.setUniforms(parametersImage);
		break;

	default:
		break;
	}
}

void SinglePassFlowFieldShaders::drawDebug() {
	int displayWidth = 320;
	int displayHeight = 180;
	fboA.draw(ofGetWindowWidth() - displayWidth, 0, displayWidth, displayHeight);
	fboB.draw(ofGetWindowWidth() - displayWidth, displayHeight, displayWidth, displayHeight);
	fboC.draw(ofGetWindowWidth() - displayWidth, displayHeight * 2, displayWidth, displayHeight);
	fboD.draw(ofGetWindowWidth() - displayWidth, displayHeight * 3, displayWidth, displayHeight);
	
	ofPushStyle();
	ofSetColor(255, 0, 0);
	for (int i = 0; i < MAX_POS; i++) {
		ofDrawCircle(
					 positions[i].x * (densityWidth / ofGetWindowWidth()),
					 positions[i].y * (densityHeight / ofGetWindowHeight()), 5);
	}
	ofPopStyle();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void SinglePassFlowFieldShaders::_update(ofEventArgs & e) {
	
	// sync positionsParameter to positions
	for (int i = 0; i < MAX_POS; i++) {
		positions[i] = positionsParameter[i].get();
	}
	if (loadShaderNextFrame) {
		reloadShaders();
		loadShaderNextFrame = false;
	}

	int currTime = ofGetElapsedTimeMillis();

	if (((currTime - lastTimeCheckMillis) > millisBetweenFileCheck) && !loadShaderNextFrame) {
		if (filesChanged()) {
			loadShaderNextFrame = true;
		}

		lastTimeCheckMillis = currTime;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
bool SinglePassFlowFieldShaders::reloadShaders() {
	ofFile commonFile;
	commonFile.open("shaders/common.glsl");
	string common = commonFile.readToBuffer().getText();
	switch (changeBuffer) {
	case ofxShadertoy::Buffer::BufferA:
		return makeShader(common + "\n" + openFile(fileA, fileAFileName), bufAShader);
		break;
//	case ofxShadertoy::Buffer::BufferB:
//		return makeShader(common + "\n" + openFile(fileB, fileBFileName), bufBShader);
//		break;
//	case ofxShadertoy::Buffer::BufferC:
//		return makeShader(common + "\n" + openFile(fileC, fileCFileName), bufCShader);
//		break;
//	case ofxShadertoy::Buffer::BufferD:
//		return makeShader(common + "\n" + openFile(fileD, fileDFileName), bufDShader);
//		break;
	case ofxShadertoy::Buffer::Image:
		return makeShader(common + "\n" + openFile(fileImage, fileImageFileName), mainShader);
		break;

	default:
		break;
	}
	return false;
}

string SinglePassFlowFieldShaders::openFile(ofFile file, string fileName) {
	file.open(fileName);
	return file.readToBuffer().getText();
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void SinglePassFlowFieldShaders::enableWatchFiles() {
	if (!bWatchingFiles) {
		ofAddListener(ofEvents().update, this, &SinglePassFlowFieldShaders::_update);
		bWatchingFiles = true;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void SinglePassFlowFieldShaders::disableWatchFiles() {
	if (bWatchingFiles) {
		ofRemoveListener(ofEvents().update, this, &SinglePassFlowFieldShaders::_update);
		bWatchingFiles = false;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
bool SinglePassFlowFieldShaders::filesChanged() {
	bool fileChanged = false;

	if (fileA.exists()) {
		std::time_t fileAShaderFileLastChangeTime = getLastModified(fileA);
		if (fileAShaderFileLastChangeTime != fileChangedTimes.at(0)) {
			fileChangedTimes.at(0) = fileAShaderFileLastChangeTime;
			fileChanged = true;
			changeBuffer = Buffer::BufferA;
		}
	}

	if (fileB.exists()) {
		std::time_t fileBShaderFileLastChangeTime = getLastModified(fileB);
		if (fileBShaderFileLastChangeTime != fileChangedTimes.at(1)) {
			fileChangedTimes.at(1) = fileBShaderFileLastChangeTime;
			fileChanged = true;
			changeBuffer = Buffer::BufferB;
		}
	}

	if (fileC.exists()) {
		std::time_t fileCShaderFileLastChangeTime = getLastModified(fileC);
		if (fileCShaderFileLastChangeTime != fileChangedTimes.at(2)) {
			fileChangedTimes.at(2) = fileCShaderFileLastChangeTime;
			fileChanged = true;
			changeBuffer = Buffer::BufferC;
		}
	}

	if (fileD.exists()) {
		std::time_t fileDShaderFileLastChangeTime = getLastModified(fileD);
		if (fileDShaderFileLastChangeTime != fileChangedTimes.at(3)) {
			fileChangedTimes.at(3) = fileDShaderFileLastChangeTime;
			fileChanged = true;
			changeBuffer = Buffer::BufferD;
		}
	}

	if (fileImage.exists()) {
		std::time_t fileImageShaderFileLastChangeTime = getLastModified(fileImage);
		if (fileImageShaderFileLastChangeTime != fileChangedTimes.at(4)) {
			fileChangedTimes.at(4) = fileImageShaderFileLastChangeTime;
			fileChanged = true;
			changeBuffer = Buffer::Image;
		}
	}
	//
	//	if( fragmentShaderFile.exists() )
	//	{
	//		std::time_t fragmentShaderFileLastChangeTime = getLastModified( fragmentShaderFile );
	//		if( fragmentShaderFileLastChangeTime != fileChangedTimes.at(1) )
	//		{
	//			fileChangedTimes.at(1) = fragmentShaderFileLastChangeTime;
	//			fileChanged = true;
	//		}
	//	}
	//
	//
	//	if( geometryShaderFile.exists() )
	//	{
	//		std::time_t geometryShaderFileLastChangeTime = getLastModified( geometryShaderFile );
	//		if( geometryShaderFileLastChangeTime != fileChangedTimes.at(2) )
	//		{
	//			fileChangedTimes.at(2) = geometryShaderFileLastChangeTime;
	//			fileChanged = true;
	//		}
	//	}

	return fileChanged;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
std::time_t SinglePassFlowFieldShaders::getLastModified(ofFile & _file) {
	if (_file.exists()) {
		// https://www.reddit.com/r/cpp_questions/comments/x00hr2/get_last_write_time_of_file/
		std::filesystem::file_time_type ftime = std::filesystem::last_write_time(_file.pathFS());
		return std::chrono::duration_cast<std::chrono::seconds>(ftime.time_since_epoch()).count();
	} else {
		return 0;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
//
void SinglePassFlowFieldShaders::setMillisBetweenFileCheck(int _millis) {
	millisBetweenFileCheck = _millis;
}
