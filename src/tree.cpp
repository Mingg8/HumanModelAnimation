#define GL_SILENCE_DEPRECATION
#include "../include/tree.h"

const char* head = "head";
const char* pelvis = "pelvis";
const char* lfoot = "lfoot";
const char* rfoot = "rfoot";
const char* ltoes = "ltoes";
const char* rtoes = "rtoes";
const char* rhand = "rhand";
const char* lhand = "lhand";

Tree::Tree(Mode _mode, const string _file) {
    mode = _mode;
    if (mode == BVH) {
        load(_file);
    } else {
    // Load the skeleton only
        load("../MotionData/Trial000.bvh");
        int i = 0;
        setVector(root_joint, i);
    }
}

void Tree::loadHierarchy(std::istream& stream) {
    std::string tmp;
    while(stream.good()) {
        stream >> tmp;
        if (trim(tmp)=="ROOT") {
            loadJoint(stream, nullptr);
        }
        else if(mode == BVH && trim(tmp) == "MOTION")
            loadMotion(stream);
    }
}

void Tree::loadJoint(std::istream& stream, Joint* parent) {
    std::string* name = new std::string;
    stream >> *name;

    Joint* joint;
    if (parent == nullptr) {
        root_joint = new Joint();
        joint = root_joint;
    } else {
        joint = new Joint();
        joint->setParent(parent);
    }
    joint->joint_name = name->c_str();
    
    std::string tmp;
    Node* node;
    const char* head = "head";
    if (strcmp(head, joint->joint_name) == false ||
        strcmp(lhand, joint->joint_name) == false ||
        strcmp(rhand, joint->joint_name) == false) {
        node = new SphereNode(body_num, default_size);
    } else if (strcmp(pelvis, joint->joint_name) == false ||
               strcmp(lfoot, joint->joint_name) == false ||
               strcmp(rfoot, joint->joint_name) == false ||
               strcmp(ltoes, joint->joint_name) == false ||
               strcmp(rtoes, joint->joint_name) == false) {
        node = new BoxNode(body_num, default_size);
    } else {
        node = new CylinderNode(body_num, default_size);
    }

    body_num ++;
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
            motionData.num_motion_channels += num;        
        }
        else if( tmp == "JOINT" )
        {
            // loading child joint and setting this as a parent
            loadJoint(stream, joint);
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
            return;
        }
    }
}

void Tree::loadMotion(std::istream& stream)
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
            int num_channels = motionData.num_motion_channels;
            // creating motion data array
            motionData.data.resize(num_frames, num_channels);
            // foreach frame read and store floats
            for (int frame = 0; frame < num_frames; frame++ )
            {
                for (int channel = 0; channel < num_channels; channel++)
                {
                    // reading float
                    float x;
                    std::stringstream ss;
                    stream >> tmp;
                    ss << tmp;
                    ss >> x;
                    
                    // calculating index for storage
                    int index = frame * num_channels + channel;
                    motionData.data(frame, channel) = x;
                }
                int data_index = 0;
                sendDataToJoint(root_joint, frame, data_index);
            }
        }
    }
}

void Tree::setVector(Joint* joint, int &data_index) {
    joints.push_back(joint);
    joint->channel_start_idx = data_index;
    data_index += joint->getNumChannels();
    
    vector<Joint*> children = joint->getChildren();
    for (int i = 0; i < children.size(); i++) {
        setVector(children[i], data_index);
    }
}

void Tree::sendDataToJoint(Joint* joint, int frame, int &data_index) {
    for (int i=0; i<joint->getNumChannels(); i++)
        joint->motion.push_back(
            motionData.data(frame,data_index+i));
    data_index = data_index + joint->getNumChannels();
    vector<Joint*> children = joint->getChildren();
    for (int i=0; i<children.size(); i++) {
        sendDataToJoint(children[i], frame, data_index);
    }
}

void Tree::drawMyHuman(Joint *joint, int frame)
{
    glPushMatrix();
    joint->transform(frame);
    if (joint->joint_name != "EndSite") {
        (joint->getNode())->draw();
    }
    vector<Joint*> children_vec = joint->getChildren();
    for (size_t i=0; i<children_vec.size(); i++) {
        Joint *j = children_vec[i];
        drawMyHuman(j, frame);
    }
    glPopMatrix();
}

Joint* Tree::getRoot()
{
    return root_joint;
}

void Tree::load(const std::string& filename)
{
    std::fstream file;
    file.open(filename.c_str(), std::ios_base::in);
    
    if(file.is_open()) {
        std::string line;
        while(file.good()) {
            file >> line;
            if (trim(line) == "HIERARCHHY")
                cout << "hierarchy" << endl;
                // TODO: error occurs if this cout is erased
                //      memory problem!!!! IDK :( :(
                loadHierarchy(file);
            break;
        }
        file.close();
    } else {
        cout << "Wrong Filepath" << endl;
    }
}


void Tree::setAngle(VectorXd ang_vel, double sec) {
//    cout << "angle: " << angle.transpose() << endl;
    int index = 0;
    for (int i = 0; i < joints.size(); i++) {
        int channel_num = joints[i]->getNumChannels();
        for (int j = 0; j < channel_num; j++) {
            (joints[i]->current_angle)(j) += ang_vel(index+j)*sec;
        }
        index += channel_num;
    }
}
