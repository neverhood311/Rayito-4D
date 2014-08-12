#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "RMaterial.h"
#include "RScene.h"
#include <vector>

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
    std::map<std::string, Rayito::Material> m_materials;
    //camera

    //map of shapes: shape name => shape
    std::map<std::string, Rayito::Shape> m_shapes;
    //mape of transforms: shape name => array of transform lists
    std::map<std::string, TransformList> m_transforms;
};

//this loads the RSD file into the SceneBuffer
class RSDLoader{
public:
    RSDLoader(SceneBuffer* _sceneBuffer, std::string filepath){
        m_scene = _sceneBuffer;
        m_filepath = filepath;
        //TODO: check if the filepath is valid
    };

    ~RSDLoader(){};

    bool Load(){
        //get the materials
        //get the camera
        //get the shapes
        //get the transforms
            //linearly interpolate between keyframes
    }

private:
    SceneBuffer* m_scene;
    std::string m_filepath;
};


#endif // SCENELOADER_H
