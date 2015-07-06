	
#include "STLParser.h"
	
//constructor definition
STLParser::STLParser(std::string filePath): mFilePath{filePath} {
	//check the endian ordering of the machine
	endianCheck();

	//attempt to open the file
	if (!openFile()){
		//TODO return FileFailedToOpen error code
	}
	//skip the header; 80 bytes. Could check for the word 'solid' to see if ascii STL file.
	mInFile.ignore(80);
	//get num of triangles; 4 bytes
	mNumberOfFacets = readUnsignedInt();
}

STLParser::~STLParser(){
	//close the file upon destruction
	mInFile.close();
}

//class member definitions
Facet STLParser::getNextFacet(){
	
	//container to hold the points that will be used to construct facet object
	std::vector<ThreeDPoint> threePoints{};
	
	//get a triangle
	//ignore the normal vector; 12 bytes
	mInFile.ignore(12);

	//create three points (four if want normal)
	int i;
	for (i=0; i<3; i++){
		float x = readFloat();
		float y = readFloat();
		float z = readFloat();
		 
		threePoints.emplace_back(x, y, z);

	}
	//end for

	//skip the attribute; 2 bytes
	mInFile.ignore(2);

	//advance facet number
	++mNextFacetNumber;

	//create and return facet object
	Facet myFacet{threePoints[0], threePoints[1], threePoints[2]};
	return myFacet;
}

uint32_t STLParser::readUnsignedInt(){
	//http://www.cplusplus.com/forum/beginner/134169/
	uint8_t bytesRead = 0;
	uint8_t uintBuffer[4];
	//TODO avoid this c-style cast
	mInFile.read((char *)uintBuffer, 4);
	bytesRead = mInFile.gcount();
	while (bytesRead < 4){
		mInFile.read((char *)uintBuffer, (4-bytesRead));
		bytesRead = mInFile.gcount();
	}	

	//if little endian
	if (!mBigEndian){
		return static_cast<uint32_t>((uintBuffer[0]) | (uintBuffer[1]<<8) | (uintBuffer[2]<<16) | (uintBuffer[3]<<24));
	} else {
		return static_cast<uint32_t>((uintBuffer[3]) | (uintBuffer[2]<<8) | (uintBuffer[1]<<16) | (uintBuffer[0]<<24));
	}
}

float STLParser::readFloat(){
	std::cout.precision(10);
	uint8_t bytesRead = 0;
	uint8_t uintBuffer[4];
	//TODO avoid this c-style cast
	mInFile.read((char *)uintBuffer, 4);
	bytesRead = mInFile.gcount();
	while (bytesRead < 4){
		mInFile.read((char *)uintBuffer, (4-bytesRead));
		bytesRead = mInFile.gcount();
	}	
	
	void *myVoidPointer = uintBuffer;
	return (*((float*)(myVoidPointer)));
	/*
	//if little endian
	if (!mBigEndian){
		return static_cast<float>((uintBuffer[0]) | (uintBuffer[1]<<8) | (uintBuffer[2]<<16) | (uintBuffer[3]<<24));
	} else {
		return static_cast<float>((uintBuffer[3]) | (uintBuffer[2]<<8) | (uintBuffer[1]<<16) | (uintBuffer[0]<<24));
	}
	*/
}

bool STLParser::openFile(){
	mInFile.open(mFilePath, std::ios_base::in | std::ios_base::binary);
	if (!mInFile){
		//TODO throw FileFailedToOpen error upon open failure
		std::cerr << "Error: unable to open the file for reading.\n";	
		mFileOpen = false;
	} else {
		std::cout << "Was able to open the file for reading.\n";
		mFileOpen = true;
	}	
	return mFileOpen;
}

void STLParser::endianCheck(){
	uint32_t testInt = 0xFFFFFF01;
	uint8_t *pTestInt = (uint8_t *) &testInt;
	if ((*pTestInt) == 0x01){
		//this machine is little endian
		mBigEndian = false; //default is true;
	}
	//printf("%x\n", ((int) (*pTestInt)));
}

uint32_t STLParser::getNextFacetNumber() const{
	return mNextFacetNumber;
}

uint32_t STLParser::getNumberOfFacets() const{
	return mNumberOfFacets;
}
	