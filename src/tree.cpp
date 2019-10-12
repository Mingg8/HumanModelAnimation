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
//            cout << "root" << endl;
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
//    cout << joint->joint_name << endl;
    
    std::string tmp;
    BoxNode* node = new BoxNode(body_num, default_size);

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
            joint->offset = (joint->offset)*resize;
            
            if (parent != nullptr) {
                (parent->getNode())->resize(joint->offset);
            }
        }
        else if (tmp == "CHANNELS") {
            stream >> joint->num_channels;
            // adding to motiondata
            motionData.num_motion_channels += joint->num_channels;
            
        }
        else if( tmp == "JOINT" )
        {
            // loading child joint and setting this as a parent
            Joint* tmp_joint = loadJoint(stream, joint);

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
               
               tmp_joint->offset = (tmp_joint->offset)*resize;
               (parent->getNode())->resize(tmp_joint->offset);
            // TODO: modify size
           }
           stream >> tmp;
        }
        else if( tmp == "}" ) {
//            cout << "end: - " << joint->joint_name << endl;
            return joint;
        }
    }
}

void Tree::loadMotion(std::istream& stream)
{
    std::string tmp;
    cout << "load motion" << endl;
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
//    if (joint->joint_name != nullptr) cout << joint->joint_name << endl; // TODO: redundant?
    if (joint->joint_name != "EndSite") (joint->getNode())->draw();
    vector<Joint*> children_vec = joint->getChildren();
    for (size_t i=0; i<children_vec.size(); i++) {
        Joint *j = children_vec[i];
        glPushMatrix();
        j->transform(frame);
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
