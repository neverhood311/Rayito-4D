#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "RMaterial.h"
#include "RScene.h"
#include <vector>
#include <Rsd/Parser.h>
#include <Rsd/File.h>
#include <sstream>
using namespace RenderSpud::Rsd;

//this contains the render settings for the scene
struct RenderSettings{
    float gamma;
    float exposure;
    int pixelSamples;
    int lightSamples;
    int maxBounceDepth;
    std::string filename;
    int imgHeight;
    int imgWidth;
    int startFrame;
    int endFrame;
};

//this is the transformation for one object for one frame
struct Transform{
    //translation
    float m_trans[4];
    //rotation
    float m_rot[6]; //xy, xz, xw, yz, yw, zw
    //scale
    float m_scale[4];
    //visibility
    bool m_visible = true;
    //whether each transform is used
    bool useTrans = false;
    bool useRot = false;
    bool useScale = false;
    bool useVisible = false;
};

struct TransformList{
    //vector of transforms (frames 0 through the final frame)
    std::vector<Transform> m_transforms;
    //number of transforms
    int m_count;
};

//this stores the entire scene once it's been loaded from the file
struct SceneBuffer{
    //map of materials: material name => material
    std::map<std::string, Rayito::Material*> m_materials;
    //camera

    //map of shapes: shape name => shape
    std::map<std::string, Rayito::Shape*> m_shapes;
    //mape of transforms: shape name => array of transform lists
    std::map<std::string, TransformList> m_transforms;
    //the render settings for the scene
    RenderSettings m_renderSettings;
};

//this loads the RSD file into the SceneBuffer
class RSDLoader{
public:

    RSDLoader(SceneBuffer* _sceneBuffer, std::string filepath){
        noErrors = true;
        m_scene = _sceneBuffer;
        m_filepath = filepath;
        if(m_scene == NULL || m_filepath == ""){
            noErrors = false;
        }
        //TODO: check if the filepath is valid
        try{
            pFile = new File(m_filepath, true);
            std::cout << "Parsed RSD file \"" << m_filepath << "\"";
        }
        catch(Exception e){
            std::cout << "Error with RSD file \"" << m_filepath << "\"";
            noErrors = false;
        }
    };

    ~RSDLoader(){};

    bool Load(){
        if(!noErrors)return false;

        try{
            Reference::Ptr pRef = Reference::fromString("scene.m_materials");
            Value::Ptr pResult = pFile->find(*pRef);
            std::string resultStr;
            if(pResult->canConvertTo(Value::kTypeString)){
                resultStr = pResult->asString();
            }
            else{
                resultStr = pResult->str();
            }
            std::cout << "Here's what you're looking for, dude:" << std::endl;
            std::cout << resultStr << std::endl;
        }
        catch(Parser::ParseException& pe){
            std::cout << "Parsement failed, dude." << std::endl;
        }

        //load the RSD file

        //get the render settings
        try{
            Reference::Ptr pRef = Reference::fromString("scene.m_renderSettings");
            Value::Ptr pResult = pFile->find(*pRef);

            //TODO: check for valid settings
            m_scene->m_renderSettings.gamma = pResult->find("gamma")->asFloat();
            m_scene->m_renderSettings.exposure = pResult->find("exposure")->asFloat();
            //TODO: make sure these are non-negative
            m_scene->m_renderSettings.pixelSamples = (int)pResult->find("pixelSamples")->asInteger();
            m_scene->m_renderSettings.lightSamples = (int)pResult->find("lightSamples")->asInteger();
            m_scene->m_renderSettings.maxBounceDepth = (int)pResult->find("maxDepth")->asInteger();
            //TODO: be sure to have a default filename if there is none specified
            m_scene->m_renderSettings.filename = pResult->find("filename")->asString();
            //TODO: make sure these are greater than 0
            m_scene->m_renderSettings.imgHeight = (int)pResult->find("imgHeight")->asInteger();
            m_scene->m_renderSettings.imgWidth = (int)pResult->find("imgWidth")->asInteger();
            //TODO: make sure endFrame >= startFrame
            m_scene->m_renderSettings.startFrame = (int)pResult->find("startFrame")->asInteger();
            m_scene->m_renderSettings.endFrame = (int)pResult->find("endFrame")->asInteger();
        }
        catch(Parser::ParseException& pe){
            std::cout << "Error loading render settings" << std::endl;
        }

        //get the materials
        try{
            //get a pointer reference to the scene materials
            Reference::Ptr pRef = Reference::fromString("scene.m_materials");
            //find the reference in the file
            Value::Ptr pResult = pFile->find(*pRef);
            size_t numMaterials = pResult->size();
            //for each material
            for(int i = 0; i < numMaterials; i++){
                Value::Ptr mat = pResult->value(i);
                //get the name
                std::string name = mat->name();
                //get the color
                Value::Ptr colorPtr = mat->find("color");
                float r_val = colorPtr->value(0)->asFloat();
                float g_val = colorPtr->value(1)->asFloat();
                float b_val = colorPtr->value(2)->asFloat();
                //if it's a glossyMaterial
                if(mat->typeNameMatches("glossyMaterial")){
                    //get the roughness
                    float rough = mat->find("roughness")->asFloat();
                    Rayito::GlossyMaterial *tempGlossyMat = new Rayito::GlossyMaterial(Rayito::Color(r_val, g_val, b_val), rough);
                    m_scene->m_materials[name] = tempGlossyMat;
                }
                //if it's a diffuseMaterial
                else if(mat->typeNameMatches("diffuseMaterial")){
                    //get nothing
                    Rayito::DiffuseMaterial *tempDiffuseMat = new Rayito::DiffuseMaterial(Rayito::Color(r_val, g_val, b_val));
                    m_scene->m_materials[name] = tempDiffuseMat;
                }
            }
        }
        catch(Parser::ParseException& pe){
            std::cout << "Error loading materials" << std::endl;
        }

        //TODO: get the camera

        //get the shapes
        try{
            Reference::Ptr pRef = Reference::fromString("scene.m_shapes");
            Value::Ptr pResult = pFile->find(*pRef);
            size_t numShapes = pResult->size();
            //for each shape
            for(int i = 0; i < numShapes; i++){
                Value::Ptr shape = pResult->value(i);
                //get the name
                std::string name = shape->name();
                //if it's a shapeLight
                if(shape->typeNameMatches("shapeLight")){
                    //get the shape
                    Rayito::Shape* tempShape = getShape(shape->find("shape"));
                    //get the color
                    Value::Ptr tempColPtr = shape->find("color");
                    Rayito::Color tempCol;
                    tempCol.m_r = tempColPtr->value(0)->asFloat();
                    tempCol.m_g = tempColPtr->value(1)->asFloat();
                    tempCol.m_b = tempColPtr->value(2)->asFloat();
                    //get the power
                    float power = shape->find("power")->asFloat();
                    //make the shape
                    Rayito::ShapeLight *shapeLight = new Rayito::ShapeLight(tempShape, tempCol, power);
                    m_scene->m_shapes[name] = shapeLight;
                    continue;
                }
                //if it's a rectangleLight
                else if(shape->typeNameMatches("rectangleLight")){
                    //get the corner
                    Value::Ptr tempPtr = shape->find("corner");
                    Rayito::Point corner;
                    corner.m_x = tempPtr->value(0)->asFloat();
                    corner.m_y = tempPtr->value(1)->asFloat();
                    corner.m_z = tempPtr->value(2)->asFloat();
                    corner.m_w = tempPtr->value(3)->asFloat();
                    //get side1
                    tempPtr = shape->find("side1");
                    Rayito::Vector side1;
                    side1.m_x = tempPtr->value(0)->asFloat();
                    side1.m_y = tempPtr->value(1)->asFloat();
                    side1.m_z = tempPtr->value(2)->asFloat();
                    side1.m_w = tempPtr->value(3)->asFloat();
                    //get side2
                    tempPtr = shape->find("side2");
                    Rayito::Vector side2;
                    side2.m_x = tempPtr->value(0)->asFloat();
                    side2.m_y = tempPtr->value(1)->asFloat();
                    side2.m_z = tempPtr->value(2)->asFloat();
                    side2.m_w = tempPtr->value(3)->asFloat();
                    //get the color
                    tempPtr = shape->find("side1");
                    Rayito::Color color;
                    color.m_r = tempPtr->value(0)->asFloat();
                    color.m_g = tempPtr->value(1)->asFloat();
                    color.m_b = tempPtr->value(2)->asFloat();
                    //get the power
                    float power = shape->find("power")->asFloat();
                    //make the shape
                    Rayito::RectangleLight *tempLight = new Rayito::RectangleLight(corner, side1, side2, color, power);
                    m_scene->m_shapes[name] = tempLight;
                    continue;
                }
                //if it's a regular old shape
                else if(shape->typeNameMatches("plane") || shape->typeNameMatches("sphere") || shape->typeNameMatches("tesseract")){
                    Rayito::Shape* tempShape = getShape(shape);
                    m_scene->m_shapes[name] = tempShape;
                }
            }
        }
        catch(Parser::ParseException& pe){
            std::cout << "Error loading shapes" << std::endl;
        }

        //get the transforms
        try{
            //linearly interpolate between keyframes
            Reference::Ptr pRef = Reference::fromString("scene.m_transforms");
            Value::Ptr pResult = pFile->find(*pRef);
            size_t numTransforms = pResult->size();
            //for each transform
            for(int i = 0; i < numTransforms; i++){
                //get the name of the shape this transform corresponds to
                Value::Ptr trans = pResult->value(i);
                //get the name of the shape
                std::string shapeName = trans->find("shape")->asString();

                Value::Ptr translate = trans->find("translate");//get the translation, if there is one
                Value::Ptr rotate = trans->find("rotation");//get the rotation, if there is one
                Value::Ptr scale = trans->find("scale");//get the scale if there is one
                Value::Ptr visible = trans->find("visibility");//get the visibility if there is one

                //get the number of frames
                int startFr =   m_scene->m_renderSettings.startFrame;
                int endFr =     m_scene->m_renderSettings.endFrame;
                int frameCount = endFr - startFr;

                //get the default values for this shape
                //Transform frVal, prevVal, nextVal;

                std::vector<std::vector<float>> transVals;
                //if there's a translate channel
                if(translate){
                    getChannelFloats(translate, transVals, 4);
                }
                std::vector<std::vector<float>> rotVals;
                if(rotate){
                    getChannelFloats(rotate, rotVals, 6);
                }
                std::vector<std::vector<float>> scaleVals;
                if(scale){
                    getChannelFloats(scale, scaleVals, 4);
                }


            }
        }
        catch(Parser::ParseException& pe){
            std::cout << "Error loading transforms" << std::endl;
        }
        return true;
    }

    bool noErrors;
private:
    void getChannelFloats(Value::Ptr _channel, std::vector<std::vector<float>> &_transVals, int numVals){

        int startFr =   m_scene->m_renderSettings.startFrame;
        int endFr =     m_scene->m_renderSettings.endFrame;
        //fill the vector with the first value all the way back to the first frame
        Value::Ptr curKey = _channel->value(0);
        int frameNum = getFrameNum(curKey->name());//the current frame we're working on
        if(frameNum > startFr){
            for(int f = startFr; f < frameNum; f++){
                std::vector<float> vals(numVals, 0);
                for(int i = 0; i < numVals; i++){
                    vals[i] = curKey->value(i)->asFloat();
                }
                _transVals.push_back(vals);
            }
        }
        size_t numKeys = _channel->size();
        //for each keyframe
        for(int curK = 0; curK < numKeys - 1; curK++){
            Value::Ptr nextKey;
            //get the next keyframe values
            nextKey = _channel->value(curK + 1);
            //fill the interpolated values
            int nextFrameNum = getFrameNum(nextKey->name());
            float range = nextFrameNum - frameNum;
            for(int interp = 0; interp < range; interp++){
                float factor = (float)interp / range;
                std::vector<float> vals(numVals, 0);
                for(int i = 0; i < numVals; i++){
                    vals[i] = lerp(curKey->value(i)->asFloat(), nextKey->value(i)->asFloat(), factor);
                }
                _transVals.push_back(vals);
            }
            frameNum = nextFrameNum;
            curKey = nextKey;
        }
        //fill the rest of the values with this keyframe's value
        for(int f = frameNum; f < endFr; f++){
            std::vector<float> vals(numVals, 0);
            for(int i = 0; i < numVals; i++){
                vals[i] = curKey->value(i)->asFloat();
            }
            _transVals.push_back(vals);
        }
    }

    double lerp(double first, double second, double factor){
        return first + factor * (second - first);
    }

    int getFrameNum(std::string str){
        return stoi(str.substr(1));
    }

    Rayito::Shape* getShape(Value::Ptr _shape){
        //get the position
        Value::Ptr tempPos = _shape->find("position");
        Rayito::Point tempPt;
        tempPt.m_x = tempPos->value(0)->asFloat();
        tempPt.m_y = tempPos->value(1)->asFloat();
        tempPt.m_z = tempPos->value(2)->asFloat();
        tempPt.m_w = tempPos->value(3)->asFloat();
        //get the material
        std::string matString = _shape->find("material")->asString();
        Rayito::Material *tempMat = m_scene->m_materials[matString];
        //if it's a plane
        if(_shape->typeNameMatches("plane")){
            //get the normal
            Value::Ptr tempNorm = _shape->find("normal");
            Rayito::Vector tempVec;
            tempVec.m_x = tempNorm->value(0)->asFloat();
            tempVec.m_y = tempNorm->value(1)->asFloat();
            tempVec.m_z = tempNorm->value(2)->asFloat();
            tempVec.m_w = tempNorm->value(3)->asFloat();
            //get the bullseye
            bool bullseye = _shape->find("bullseye")->asBoolean();
            Rayito::Plane *tempPlane = new Rayito::Plane(tempPt, tempVec, tempMat, bullseye);
            return tempPlane;
        }
        //if it's a sphere
        else if(_shape->typeNameMatches("sphere")){
            //get the radius
            float radius = _shape->find("radius")->asFloat();
            Rayito::Sphere *tempSphere = new Rayito::Sphere(tempPt, radius, tempMat);
            return tempSphere;
        }
        //if it's a tesseract
        else if(_shape->typeNameMatches("tesseract")){
            //get the sidelength
            float sideLength = _shape->find("sidelength")->asFloat();
            Rayito::Tesseract *tempTess = new Rayito::Tesseract(tempPt, sideLength, tempMat);
            return tempTess;
        }
    }

    SceneBuffer* m_scene;
    std::string m_filepath;
    File::FilePtr pFile;
};


#endif // SCENELOADER_H
