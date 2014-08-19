#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "RMaterial.h"
#include "RScene.h"
#include <vector>
#include <Rsd/Parser.h>
#include <Rsd/File.h>
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
};

//this is the transformation for one object for one frame
struct Transform{
    //translation
    float m_trans[4];
    //rotation
    float m_rot[6]; //xy, xz, xw, yz, yw, zw
    //visibility
    bool m_visible = true;
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
            Reference::Ptr pRef = Reference::fromString("scene.m_renderSettings.gamma");
            Value::Ptr pResult = pFile->find(*pRef);
            m_scene->m_renderSettings.gamma = pResult->asFloat();

            pRef = Reference::fromString("scene.m_renderSettings.exposure");
            pResult = pFile->find(*pRef);
            m_scene->m_renderSettings.exposure = pResult->asFloat();

            pRef = Reference::fromString("scene.m_renderSettings.pixelSamples");
            pResult = pFile->find(*pRef);
            m_scene->m_renderSettings.pixelSamples = (int)pResult->asInteger();

            pRef = Reference::fromString("scene.m_renderSettings.lightSamples");
            pResult = pFile->find(*pRef);
            m_scene->m_renderSettings.lightSamples = (int)pResult->asInteger();

            pRef = Reference::fromString("scene.m_renderSettings.maxDepth");
            pResult = pFile->find(*pRef);
            m_scene->m_renderSettings.maxBounceDepth = (int)pResult->asInteger();

            pRef = Reference::fromString("scene.m_renderSettings.filename");
            pResult = pFile->find(*pRef);
            m_scene->m_renderSettings.filename = pResult->asString();

            pRef = Reference::fromString("scene.m_renderSettings.imgHeight");
            pResult = pFile->find(*pRef);
            m_scene->m_renderSettings.imgHeight = (int)pResult->asInteger();

            pRef = Reference::fromString("scene.m_renderSettings.imgWidth");
            pResult = pFile->find(*pRef);
            m_scene->m_renderSettings.imgWidth = (int)pResult->asInteger();
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
                    //get the color
                    //get the power
                    //get the shape
                    continue;
                }
                //if it's a rectangleLight
                else if(shape->typeNameMatches("rectangleLight")){
                    //get the corner
                    //get side1
                    //get side2
                    //get the color
                    //get the power
                    continue;
                }
                //get the position
                Value::Ptr tempPos = shape->find("position");
                Rayito::Point tempPt;
                tempPt.m_x = tempPos->value(0)->asFloat();
                tempPt.m_y = tempPos->value(1)->asFloat();
                tempPt.m_z = tempPos->value(2)->asFloat();
                tempPt.m_w = tempPos->value(3)->asFloat();
                //get the material
                std::string matString = shape->find("material")->asString();
                Rayito::Material *tempMat = m_scene->m_materials[matString];
                //if it's a plane
                if(shape->typeNameMatches("plane")){
                    //get the normal
                    Value::Ptr tempNorm = shape->find("normal");
                    Rayito::Vector tempVec;
                    tempVec.m_x = tempNorm->value(0)->asFloat();
                    tempVec.m_y = tempNorm->value(1)->asFloat();
                    tempVec.m_z = tempNorm->value(2)->asFloat();
                    tempVec.m_w = tempNorm->value(3)->asFloat();
                    //get the bullseye
                    bool bullseye = shape->find("bullseye")->asBoolean();
                    Rayito::Plane *tempPlane = new Rayito::Plane(tempPt, tempVec, tempMat, bullseye);
                    m_scene->m_shapes[name] = tempPlane;
                }
                //if it's a sphere
                else if(shape->typeNameMatches("sphere")){
                    //get the radius
                    float radius = shape->find("radius")->asFloat();
                    Rayito::Sphere *tempSphere = new Rayito::Sphere(tempPt, radius, tempMat);
                    m_scene->m_shapes[name] = tempSphere;
                }
                //if it's a tesseract
                else if(shape->typeNameMatches("tesseract")){
                    //get the sidelength
                    float sideLength = shape->find("sidelength")->asFloat();
                    Rayito::Tesseract *tempTess = new Rayito::Tesseract(tempPt, sideLength, tempMat);
                }
            }
        }
        catch(Parser::ParseException& pe){
            std::cout << "Error loading shapes" << std::endl;
        }

        //TODO: get the transforms
            //linearly interpolate between keyframes
        return true;
    }

    bool noErrors;
private:
    SceneBuffer* m_scene;
    std::string m_filepath;
    File::FilePtr pFile;
};


#endif // SCENELOADER_H
