#include "../include/bvh.h"
#include "../include/tree.h"

const char* head = "head";
const char* pelvis = "pelvis";
const char* lfoot = "lfoot";
const char* rfoot = "rfoot";
const char* ltoes = "ltoes";
const char* rtoes = "rtoes";
const char* rhand = "rhand";
const char* lhand = "lhand";

void BVH::loadHierarchy(std::istream& stream) {
}

Joint* BVH::loadJoint(std::istream& stream, Joint* parent, int& _num_motion_channels) {
    std::string* name = new std::string;
    stream >> *name;
    Joint* joint = new Joint();
    joint->setParent(parent);
    joint->joint_name = name->c_str();
    
    std::string tmp;
    Node* node;
    if (strcmp(head, joint->joint_name) == false ||
        strcmp(lhand, joint->joint_name) == false ||
        strcmp(rhand, joint->joint_name) == false) {
        node = new SphereNode(default_size);
    } else if (strcmp(pelvis, joint->joint_name) == false ||
               strcmp(lfoot, joint->joint_name) == false ||
               strcmp(rfoot, joint->joint_name) == false ||
               strcmp(ltoes, joint->joint_name) == false ||
               strcmp(rtoes, joint->joint_name) == false) {
        node = new BoxNode(default_size);
    } else {
        node = new CylinderNode(default_size);
    }
    
    joint->setNode(node);
    
    while (stream.good()) {
        stream >> tmp;
        tmp = trim(tmp);
        
        char c = tmp.at(0);
        if (c=='X' || c == 'Y' || c == 'Z') {
            if (tmp == "Xposition")
                joint->addToChannel(Joint::DIR::Xtrans);
            if (tmp == "Yposition")
                joint->addToChannel(Joint::DIR::Ytrans);
            if (tmp == "Zposition")
                joint->addToChannel(Joint::DIR::Ztrans);
            if( tmp == "Xrotation" )
                joint->addToChannel(Joint::DIR::Xrot);
            if( tmp == "Yrotation" )
                joint->addToChannel(Joint::DIR::Yrot);
            if( tmp == "Zrotation" )
                joint->addToChannel(Joint::DIR::Zrot);
        }
        if (tmp=="OFFSET") {
            stream >> joint->offset(0)
            >> joint->offset(1)
            >> joint->offset(2);
            joint->offset = (joint->offset)*resize;
            
            if (parent != nullptr) {
                (parent->getNode())->resize(joint->offset);
            }
        }
        else if (tmp == "CHANNELS") {
            int num;
            stream >> num;
            joint->setNumChannels(num);
            // adding to motiondata
            _num_motion_channels += num;
        }
        else if( tmp == "JOINT" )
        {
            // loading child joint and setting this as a parent
            loadJoint(stream, joint, _num_motion_channels);
        }
        else if( tmp == "End" )
        {
            // loading End Site joint
            stream >> tmp >> tmp;
            Joint* tmp_joint = new Joint;
            
            tmp_joint->joint_name = "EndSite";
            tmp_joint->setParent(joint);
            tmp_joint->setNumChannels(0);
            
            stream >> tmp;
            if( tmp == "OFFSET" ) {
                stream >> tmp_joint->offset(0)
                >> tmp_joint->offset(1)
                >> tmp_joint->offset(2);
                
                tmp_joint->offset = (tmp_joint->offset)*resize;
                (joint->getNode())->resize(tmp_joint->offset);
            }
            stream >> tmp;
        }
        else if( tmp == "}" ) {
            return joint;
        }
    }
}


void BVH::loadMotion(std::istream& stream, MOTION& motionData, int &_num_motion_channels)
{
    std::string tmp;
    while( stream.good() )
    {
        stream >> tmp;
        
        if( trim(tmp) == "Frames:" )
        {
            // loading frame number
            stream >> motionData.num_frames;
        }
        else if( trim(tmp) == "Frame" )
        {
            // loading frame time
            double frame_time;
            stream >> tmp >> frame_time;
            
            motionData.frame_time = frame_time;
            int num_frames   = motionData.num_frames;
            // creating motion data array
            motionData.data.resize(num_frames, _num_motion_channels);
            // foreach frame read and store floats
            for (int frame = 0; frame < num_frames; frame++ )
            {
                for (int channel = 0; channel < _num_motion_channels; channel++)
                {
                    // reading float
                    float x;
                    std::stringstream ss;
                    stream >> tmp;
                    ss << tmp;
                    ss >> x;
                    
                    motionData.data(frame, channel) = x;
                }
            }
        }
    }
}

void BVH::sendTotDataToJoint(MOTION motion, Joint* root) {
    for (int frame = 0; frame < motion.num_frames; frame++) {
        int data_index = 0;
        sendDataToJoint(root, motion, frame, data_index);
    }
}

void BVH::sendDataToJoint(Joint* joint, MOTION& motion, int frame, int &data_index) {
    for (int i=0; i<joint->getNumChannels(); i++)
        joint->motion.push_back(motion.data(frame,data_index+i));
    data_index = data_index + joint->getNumChannels();
    vector<Joint*> children = joint->getChildren();
    for (int i=0; i<children.size(); i++) {
        sendDataToJoint(children[i], motion, frame, data_index);
    }
}

int BVH::loadWhole(const std::string& filename, MOTION& motion, Joint*& root)
{
    std::fstream file;
    file.open(filename.c_str(), std::ios_base::in);
    
    if(file.is_open()) {
        std::string line;
        while(file.good()) {
            file >> line;
            if (trim(line) == "HIERARCHHY")
                cout << "hierarchy" << endl;
            std::string tmp;
            while(file.good()) {
                file >> tmp;
                if (trim(tmp)=="ROOT") {
                    num_motion_channels = 0;
                    root = loadJoint(file, nullptr, num_motion_channels);
                }
                else if(trim(tmp) == "MOTION")
                    loadMotion(file, motion, num_motion_channels);
            }
            break;
        }
        file.close();
    } else {
        cout << "Wrong Filepath" << endl;
    }
    return num_motion_channels;
}

int BVH::loadOnlyJoint(const std::string& filename, MOTION& motion, Joint*& root)
{
    std::fstream file;
    file.open(filename.c_str(), std::ios_base::in);
    
    if(file.is_open()) {
        std::string line;
        while(file.good()) {
            file >> line;
            if (trim(line) == "HIERARCHHY")
                cout << "hierarchy" << endl;
            std::string tmp;
            while(file.good()) {
                file >> tmp;
                if (trim(tmp)=="ROOT") {
                    num_motion_channels = 0;
                    root = loadJoint(file, nullptr, num_motion_channels);
                }
            }
            break;
        }
        file.close();
    } else {
        cout << "Wrong Filepath" << endl;
    }
    return num_motion_channels;
}
void BVH::loadOnlyMotion(const std::string& filename, MOTION& motion, Joint* root)
{
    std::fstream file;
    file.open(filename.c_str(), std::ios_base::in);
    
    if(file.is_open()) {
        std::string line;
        while(file.good()) {
            file >> line;
            if (trim(line) == "HIERARCHHY")
                cout << "hierarchy" << endl;
            std::string tmp;
            while(file.good()) {
                file >> tmp;
                if(trim(tmp) == "MOTION")
                    // get num_motion_channels from somewhere
                    loadMotion(file, motion, num_motion_channels);
            }
            break;
        }
        file.close();
    } else {
        cout << "Wrong Filepath" << endl;
    }
}
