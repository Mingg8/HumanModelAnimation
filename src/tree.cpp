#define GL_SILENCE_DEPRECATION
#include "../include/tree.h"

// TODO: change head size

Tree::Tree(const string& filename)
{
    load(filename);
}

void Tree::loadHierarchy(std::istream& stream) {
    std::cout << "load hierarchy" << endl;
    std::string tmp;
    while(stream.good()) {
        stream >> tmp;
        if (trim(tmp)=="ROOT") {
            cout << "root" << endl;
            loadJoint(stream, nullptr);
        }
        else if(trim(tmp) == "MOTION")
            loadMotion(stream);
    }
}

Joint* Tree::loadJoint(std::istream& stream, Joint* parent) {
    // load joint name
    std::string* name = new std::string;
    stream >> *name;

    Joint* joint;
    if (parent == nullptr) {
        root_joint = new Joint();
        joint = root_joint;
    } else {
        joint = new Joint();
    }
    joint->joint_name = name->c_str();
    cout << joint->joint_name << endl;
    
    std::string tmp;
    
    // setting local matrix to identity
    joint->parent_to_child << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    
    static int _channel_start = 0;
    unsigned channel_order_index = 0;

    BoxNode* node = new BoxNode(body_num, -default_size, default_size,
        -default_size, default_size, -default_size, default_size);

    body_num ++;
    if (parent != nullptr) {
        joint->setParent(parent);
    }
    joint->setNode(node);

    
    while (stream.good()) {
        stream >> tmp;
        tmp = trim(tmp);
        
        // loading channels
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

            joint->offset(0) = joint->offset(0)*resize;
            joint->offset(1) = joint->offset(1)*resize;
            joint->offset(2) = joint->offset(2)*resize;

            if (parent != nullptr) {
                if (joint->offset(0) > (parent->getNode())->maxX) {
                    (parent->getNode())->maxX = joint->offset(0);
                }
                if (joint->offset(0) < (parent->getNode())->minX) {
                    (parent->getNode())->minX = joint->offset(0);
                }
                if (joint->offset(1) > (parent->getNode())->maxY) {
                    (parent->getNode())->maxY = joint->offset(1);
                }
                if (joint->offset(1) < (parent->getNode())->minY) {
                    (parent->getNode())->minY = joint->offset(1);
                }
                if (joint->offset(2) > (parent->getNode())->maxZ) {
                    (parent->getNode())->maxZ = joint->offset(2);
                }
                if (joint->offset(2) < (parent->getNode())->minZ) {
                    (parent->getNode())->minZ = joint->offset(2);
                }
            }
        }
        else if (tmp == "CHANNELS") {
            stream >> joint->num_channels;
            
            // adding to motiondata
            motionData.num_motion_channels += joint->num_channels;
            
            // increasing static counter of channel index starting motion section
            joint->channel_start = _channel_start;
            _channel_start += joint->num_channels;            
        }
        else if( tmp == "JOINT" )
        {
            // loading child joint and setting this as a parent
            Joint* tmp_joint = loadJoint(stream, joint);
            joint->addToChildren(tmp_joint); // TODO: erase this line if is redundant

        }
        else if( tmp == "End" )
        {
            // loading End Site joint
            stream >> tmp >> tmp; // Site {
            Joint* tmp_joint = new Joint;

            tmp_joint->joint_name = "EndSite";
            tmp_joint->setParent(joint);
            tmp_joint->num_channels = 0;

           stream >> tmp;
           if( tmp == "OFFSET" ) {
              stream >> tmp_joint->offset(0)
              >> tmp_joint->offset(1)
              >> tmp_joint->offset(2);
               // TODO: modify size
           }
           stream >> tmp;
        }
        else if( tmp == "}" ) {
            cout << "end: - " << joint->joint_name << endl;
            return joint;
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
            int num_channels = motionData.num_motion_channels; // total channels
            
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

void Tree::sendDataToJoint(Joint* joint, int frame, int &data_index) {
    for (int i=0; i<joint->num_channels; i++)
        joint->motion.push_back(
            motionData.data(frame,data_index+i));
    data_index = data_index + joint->num_channels;
    vector<Joint*> children = joint->getChildren();
    for (int i=0; i<children.size(); i++) {
        sendDataToJoint(children[i], frame, data_index);
    }
}

void Tree::drawMyHuman(Joint *joint, int frame)
{
    if (joint->getNode() != nullptr) (joint->getNode())->draw();
    vector<Joint*> children_vec = joint->getChildren();
    // std::cout << "children num: " << children_vec.size() << endl;
    for (size_t i=0; i<children_vec.size(); i++) {
        Joint *j = children_vec[i];
        // cout << j->joint_name << endl;
        glPushMatrix();
        j->transform();
        drawMyHuman(children_vec[i], frame);
        glPopMatrix();
    }
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
            cout << "loaded" << endl;
            file >> line;
            if (trim(line) == "HIERARCHHY")
                cout << "hiee" << endl;
                loadHierarchy(file);
            break;
        }
        file.close();
    } else {
        cout << "Wrong Filepath" << endl;
    }
}
