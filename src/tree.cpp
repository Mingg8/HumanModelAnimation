#define GL_SILENCE_DEPRECATION
#include "../include/tree.h"
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
                joint->channels_order[channel_order_index++] = Xposition;
            if (tmp == "Yposition")
                joint->channels_order[channel_order_index++] = Yposition;
            if (tmp == "Zposition")
                joint->channels_order[channel_order_index++] = Zposition;
            if( tmp == "Xrotation" )
                joint->channels_order[channel_order_index++] = Xrotation;
            if( tmp == "Yrotation" )
                joint->channels_order[channel_order_index++] = Yrotation;
            if( tmp == "Zrotation" )
                joint->channels_order[channel_order_index++] = Zrotation;
        }
        if (tmp=="OFFSET") {
            stream >> joint->offset.x
            >> joint->offset.y
            >> joint->offset.z;

            joint->offset.x = joint->offset.x/resize;
            joint->offset.y = joint->offset.y/resize;
            joint->offset.z = joint->offset.z/resize;

            if (parent != nullptr) {
                if (joint->offset.x > (parent->getNode())->maxX) {
                    (parent->getNode())->maxX = joint->offset.x;
                }
                if (joint->offset.x < (parent->getNode())->minX) {
                    (parent->getNode())->minX = joint->offset.x;
                }
                if (joint->offset.y > (parent->getNode())->maxY) {
                    (parent->getNode())->maxY = joint->offset.y;
                }
                if (joint->offset.y < (parent->getNode())->minY) {
                    (parent->getNode())->minY = joint->offset.y;
                }
                if (joint->offset.z > (parent->getNode())->maxZ) {
                    (parent->getNode())->maxZ = joint->offset.z;
                }
                if (joint->offset.z < (parent->getNode())->minZ) {
                    (parent->getNode())->minZ = joint->offset.z;
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
            
            // creating array for channel order specification
            joint->channels_order = new short[joint->num_channels];
            
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
              stream >> tmp_joint->offset.x
              >> tmp_joint->offset.y
              >> tmp_joint->offset.z;
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
            float frame_time;
            stream >> tmp >> frame_time;
            
            int num_frames   = motionData.num_frames;
            int num_channels = motionData.num_motion_channels;
            
            // creating motion data array
            motionData.data = new float[num_frames * num_channels];
            
            // foreach frame read and store floats
            for( int frame = 0; frame < num_frames; frame++ )
            {
                for( int channel = 0; channel < num_channels; channel++)
                {
                    // reading float
                    float x;
                    std::stringstream ss;
                    stream >> tmp;
                    ss << tmp;
                    ss >> x;
                    
                    // calculating index for storage
                    int index = frame * num_channels + channel;
                    motionData.data[index] = x;
                }
            }
        }
    }
}

void Tree::drawMyHuman(Joint *joint)
{
    if (joint->getNode() != nullptr) (joint->getNode())->draw();
    vector<Joint*> children_vec = joint->getChildren();
    // std::cout << "children num: " << children_vec.size() << endl;
    for (size_t i=0; i<children_vec.size(); i++) {
        Joint *j = children_vec[i];
        // cout << j->joint_name << endl;
        glPushMatrix();
//        j->transform();
        drawMyHuman(children_vec[i]);
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
