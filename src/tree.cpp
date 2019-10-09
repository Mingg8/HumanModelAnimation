#define GL_SILENCE_DEPRECATION
#include "../include/tree.h"
Tree::Tree()
{
    pelvis_radius = 0.15/2;
    foot_height = 0.05;
    leg_height = 0.5;
    leg_radius = 0.15/2;
    foot_width = 0.10;
    foot_length = 0.25;
    upper_body_height = 0.6;
    upper_body_depth = 0.15;;
    upper_body_width = 0.4;
    head_radius = 0.15;
    leg_offset = 0.05+0.15/2;
    arm_radius = 0.05;
    arm_length = 0.4;
    hand_radius = 0.08;
    pelvis_offset = 0.08;

    body_root = new Node(0);
}



void Tree::setUpMyHuman()
{
    double pelvis_height = 0.0;
    Eigen::Matrix4d parent_joint, joint_child;
    parent_joint.setIdentity();
    joint_child << 0, 0, -1, 0,
    0, 1, 0, 0,
    1, 0, 0, pelvis_height,
    0, 0, 0, 1;
    
    // pelvis
    Floating* joint_01 = new Floating(parent_joint, joint_child);
    CylinderNode* body_1 = new CylinderNode(1, pelvis_radius, upper_body_width);
    body_1->setParent(body_root, joint_01);
    
    // right upper leg
    CylinderNode* body_2 = new CylinderNode(2, leg_radius, leg_height);
    parent_joint << 1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, (leg_offset+upper_body_width/2),
                    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
                    0, 0, 1, -(leg_height+pelvis_radius),
                    0, -1, 0, 0,
                    0, 0, 0, 1;
    BallSocket* joint_12 = new BallSocket(parent_joint, joint_child,
                                          0.0, 15.0, 0.0, 0.0, -15.0, 15.0);
    body_2->setParent(body_1, joint_12);

    // left upper leg
    CylinderNode* body_3 = new CylinderNode(3, leg_radius, leg_height);
    parent_joint(2, 3) = upper_body_width/2-leg_offset;
    BallSocket* joint_13 = new BallSocket(parent_joint, joint_child,
                                          0.0, 15.0, 0.0, 0.0, -15.0, 15.0);
    body_3->setParent(body_1, joint_13);

    // right lower leg
    CylinderNode* body_12 = new CylinderNode(12, leg_radius, leg_height);
    parent_joint << 1, 0, 0, 0,
                   0, 0, -1, 0,
                   0, 1, 0, 0,
                   0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
                0, 0, 1, -leg_height,
                0, -1, 0, 0,
                0, 0, 0, 1;
    Revolute* joint_2_12 = new Revolute(parent_joint, joint_child, -30.0, 0.0);
    body_12->setParent(body_2, joint_2_12);

    // left lower leg
    CylinderNode* body_14 = new CylinderNode(14, leg_radius, leg_height);
    Revolute* joint_3_14 = new Revolute(parent_joint, joint_child, -30.0, 0.0);
    body_14->setParent(body_3, joint_3_14);

    // right foot
    BoxNode* body_13 = new BoxNode(13, foot_width, foot_height, foot_length);
    parent_joint << 1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1;
    joint_child << 0, 0, 1, foot_length/2,
                    0, 1, 0, 0,
                    -1, 0, 0, 0,
                    0, 0, 0, 2;
    Revolute* joint_12_13 = new Revolute(parent_joint, joint_child, -30.0, 30.0);
    body_13->setParent(body_12, joint_12_13);

    // left foot
    BoxNode* body_15 = new BoxNode(15, foot_width, foot_height, foot_length);
    Revolute* joint_14_15 = new Revolute(parent_joint, joint_child, -30.0, 30.0);
    body_15->setParent(body_14, joint_14_15);

    // body
    BoxNode* body_4 = new BoxNode(4, upper_body_width, upper_body_height, upper_body_depth);
    parent_joint << 0, -1, 0, 0,
                    1, 0, 0, 0,
                    0, 0, 1, upper_body_width/2,
                    0, 0, 0, 1;
    joint_child << 0, 1, 0, upper_body_height/2+pelvis_offset,
                    0, 0, -1, 0,
                    -1, 0, 0, 0,
                    0, 0, 0, 1;
    BallSocket* joint_1_4 = new BallSocket(parent_joint, joint_child, -30.0, 0.0, -3.0, 3.0, -10.0, 10.0);
    body_4->setParent(body_1, joint_1_4);

    // right arm
    CylinderNode* body_6 = new CylinderNode(6, arm_radius, arm_length);
    parent_joint << -1, 0, 0, -(upper_body_width/2+arm_radius),
                    0, -1, 0, upper_body_height/2,
                    0, 0, 1, 0,
                    0, 0, 0, 1;
    joint_child << 0, -1, 0, 0,
                    0, 0, -1, arm_length,
                    1, 0, 0, 0,
                    0, 0, 0, 1;
    BallSocket* joint_4_6 = new BallSocket(parent_joint, joint_child, -15.0, 0.0, 0.0, 0.0, -15.0, 15.0);
    body_6->setParent(body_4, joint_4_6);

    // left arm
    CylinderNode* body_7 = new CylinderNode(7, arm_radius, arm_length);
    parent_joint << 1, 0, 0, (upper_body_width/2+arm_radius),
                    0, 1, 0, upper_body_height/2,
                    0, 0, 1, 0,
                    0, 0, 0, 1;
    joint_child << 0, 1, 0, 0,
                    0, 0, 1, -arm_length,
                    1, 0, 0, 0,
                    0, 0, 0, 1;
    BallSocket* joint_4_7 = new BallSocket(parent_joint, joint_child, 0.0, 15.0, 0.0, 0.0, -15.0, 15.0);
    body_7->setParent(body_4, joint_4_7);

    // right lower arm
    CylinderNode* body_8 = new CylinderNode(8, arm_radius, arm_length);
    parent_joint << 1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
                    0, 0, 1, -arm_length,
                    0, -1, 0, 0,
                    0, 0, 0, 1;
    BallSocket *joint_6_8 = new BallSocket(parent_joint, joint_child, 0.0, 0.0, 0.0, 0.0,  0.0, 15.0);
    body_8->setParent(body_6, joint_6_8);

    // left lower arm
    CylinderNode* body_10 = new CylinderNode(10, arm_radius, arm_length);
    parent_joint << 0, 1, 0, 0,
                    0, 0, 1, 0,
                    1, 0, 0, 0,
                    0, 0, 0, 1;
    joint_child << 0, 0, 1, -arm_length,
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1;
    Revolute *joint_7_10 = new Revolute(parent_joint, joint_child, -15.0, 0.0);
    body_10->setParent(body_7, joint_7_10);

    // head
    SphereNode* body_5 = new SphereNode(5, head_radius);
    parent_joint << 1, 0, 0, 0,
    0, 0, 1, upper_body_height/2,
    0, -1, 0, 0,
    0, 0, 0, 1;
    joint_child << 0, 1, 0, 0,
    -1, 0, 0, 0,
    0, 0, 1, head_radius,
    0, 0, 0, 1;
    BallSocket *joint_4_5 = new BallSocket(parent_joint, joint_child, -30.0, 30.0 , 0.0, 0.0, -30.0, 30.0);
    body_5->setParent(body_4, joint_4_5);

    // right hand
    SphereNode* body_9 = new SphereNode(9, hand_radius);
    parent_joint << 1, 0, 0, 0,
    0, 0, 1, 0,
    0, -1, 0, -hand_radius,
    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1;
    BallSocket *joint_8_9 = new BallSocket(parent_joint, joint_child, -30.0, 30.0, 0.0, 0.0, -30.0, 30.0);
    body_9->setParent(body_8, joint_8_9);

    // left hand
    SphereNode* body_11 = new SphereNode(11, hand_radius);
    parent_joint << 1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, -hand_radius,
    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
    BallSocket *joint_10_11 = new BallSocket(parent_joint, joint_child, -30.0, 30.0, 0.0, 0.0, -30.0, 30.0);
    body_11->setParent(body_10, joint_10_11);
}

void Tree::setUpMyHuman2()
{
    // Body First
    double pelvis_height = 0.0;
    
    Eigen::Matrix4d parent_joint, joint_child;
    parent_joint.setIdentity();
    joint_child << 0, 0, -1, 0,
    0, 1, 0, 0,
    1, 0, 0, pelvis_height,
    0, 0, 0, 1;
    
    // pelvis
    Floating* joint_01 = new Floating(parent_joint, joint_child);
    CylinderNode* body_1 = new CylinderNode(1, pelvis_radius, upper_body_width);
    body_1->setParent(body_root, joint_01);
    
    // right upper leg
    CylinderNode* body_2 = new CylinderNode(2, leg_radius, leg_height);
    parent_joint << 1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, (leg_offset+upper_body_width/2),
    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
    0, 0, 1, -(leg_height+pelvis_radius),
    0, -1, 0, 0,
    0, 0, 0, 1;
    BallSocket* joint_12 = new BallSocket(parent_joint, joint_child,
                                          0.0, 15.0, 0.0, 0.0, -15.0, 15.0);
    body_2->setParent(body_1, joint_12);
    
    // left upper leg
    CylinderNode* body_3 = new CylinderNode(3, leg_radius, leg_height);
    parent_joint(2, 3) = upper_body_width/2-leg_offset;
    BallSocket* joint_13 = new BallSocket(parent_joint, joint_child,
                                          0.0, 15.0, 0.0, 0.0, -15.0, 15.0);
    body_3->setParent(body_1, joint_13);
    
    // right lower leg
    CylinderNode* body_12 = new CylinderNode(12, leg_radius, leg_height);
    parent_joint << 1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, 0,
    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
    0, 0, 1, -leg_height,
    0, -1, 0, 0,
    0, 0, 0, 1;
    Revolute* joint_2_12 = new Revolute(parent_joint, joint_child, -30.0, 0.0);
    body_12->setParent(body_2, joint_2_12);
    
    // left lower leg
    CylinderNode* body_14 = new CylinderNode(14, leg_radius, leg_height);
    Revolute* joint_3_14 = new Revolute(parent_joint, joint_child, -30.0, 0.0);
    body_14->setParent(body_3, joint_3_14);
    
    // right foot
    BoxNode* body_13 = new BoxNode(13, foot_width, foot_height, foot_length);
    parent_joint << 1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, 0,
    0, 0, 0, 1;
    joint_child << 0, 0, 1, foot_length/2,
    0, 1, 0, 0,
    -1, 0, 0, 0,
    0, 0, 0, 2;
    Revolute* joint_12_13 = new Revolute(parent_joint, joint_child, -30.0, 30.0);
    body_13->setParent(body_12, joint_12_13);
    
    // left foot
    BoxNode* body_15 = new BoxNode(15, foot_width, foot_height, foot_length);
    Revolute* joint_14_15 = new Revolute(parent_joint, joint_child, -30.0, 30.0);
    body_15->setParent(body_14, joint_14_15);
    
    // body
    BoxNode* body_4 = new BoxNode(4, upper_body_width, upper_body_height, upper_body_depth);
    parent_joint << 0, -1, 0, 0,
    1, 0, 0, 0,
    0, 0, 1, upper_body_width/2,
    0, 0, 0, 1;
    joint_child << 0, 1, 0, upper_body_height/2+pelvis_offset,
    0, 0, -1, 0,
    -1, 0, 0, 0,
    0, 0, 0, 1;
    BallSocket* joint_1_4 = new BallSocket(parent_joint, joint_child, -30.0, 0.0, -3.0, 3.0, -10.0, 10.0);
    body_4->setParent(body_1, joint_1_4);
    
    // right arm
    CylinderNode* body_6 = new CylinderNode(6, arm_radius, arm_length);
    parent_joint << -1, 0, 0, -(upper_body_width/2+arm_radius),
    0, -1, 0, upper_body_height/2,
    0, 0, 1, 0,
    0, 0, 0, 1;
    joint_child << 0, -1, 0, 0,
    0, 0, -1, arm_length,
    1, 0, 0, 0,
    0, 0, 0, 1;
    BallSocket* joint_4_6 = new BallSocket(parent_joint, joint_child, -15.0, 0.0, 0.0, 0.0, -15.0, 15.0);
    body_6->setParent(body_4, joint_4_6);
    
    // left arm
    CylinderNode* body_7 = new CylinderNode(7, arm_radius, arm_length);
    parent_joint << 1, 0, 0, (upper_body_width/2+arm_radius),
    0, 1, 0, upper_body_height/2,
    0, 0, 1, 0,
    0, 0, 0, 1;
    joint_child << 0, 1, 0, 0,
    0, 0, 1, -arm_length,
    1, 0, 0, 0,
    0, 0, 0, 1;
    BallSocket* joint_4_7 = new BallSocket(parent_joint, joint_child, 0.0, 15.0, 0.0, 0.0, -15.0, 15.0);
    body_7->setParent(body_4, joint_4_7);
    
    // right lower arm
    CylinderNode* body_8 = new CylinderNode(8, arm_radius, arm_length);
    parent_joint << 1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, 0,
    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
    0, 0, 1, -arm_length,
    0, -1, 0, 0,
    0, 0, 0, 1;
    BallSocket *joint_6_8 = new BallSocket(parent_joint, joint_child, 0.0, 0.0, 0.0, 0.0,  0.0, 15.0);
    body_8->setParent(body_6, joint_6_8);
    
    // left lower arm
    CylinderNode* body_10 = new CylinderNode(10, arm_radius, arm_length);
    parent_joint << 0, 1, 0, 0,
    0, 0, 1, 0,
    1, 0, 0, 0,
    0, 0, 0, 1;
    joint_child << 0, 0, 1, -arm_length,
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 0, 1;
    Revolute *joint_7_10 = new Revolute(parent_joint, joint_child, -15.0, 0.0);
    body_10->setParent(body_7, joint_7_10);
    
    // head
    SphereNode* body_5 = new SphereNode(5, head_radius);
    parent_joint << 1, 0, 0, 0,
    0, 0, 1, upper_body_height/2,
    0, -1, 0, 0,
    0, 0, 0, 1;
    joint_child << 0, 1, 0, 0,
    -1, 0, 0, 0,
    0, 0, 1, head_radius,
    0, 0, 0, 1;
    BallSocket *joint_4_5 = new BallSocket(parent_joint, joint_child, -30.0, 30.0 , 0.0, 0.0, -30.0, 30.0);
    body_5->setParent(body_4, joint_4_5);
    
    // right hand
    SphereNode* body_9 = new SphereNode(9, hand_radius);
    parent_joint << 1, 0, 0, 0,
    0, 0, 1, 0,
    0, -1, 0, -hand_radius,
    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1;
    BallSocket *joint_8_9 = new BallSocket(parent_joint, joint_child, -30.0, 30.0, 0.0, 0.0, -30.0, 30.0);
    body_9->setParent(body_8, joint_8_9);
    
    // left hand
    SphereNode* body_11 = new SphereNode(11, hand_radius);
    parent_joint << 1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, -hand_radius,
    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1;
    BallSocket *joint_10_11 = new BallSocket(parent_joint, joint_child, -30.0, 30.0, 0.0, 0.0, -30.0, 30.0);
    body_11->setParent(body_10, joint_10_11);
    
    // set up the joints
    
}

void Tree::loadHierarchy(std::istream& stream) {
    std::string tmp;
    while(stream.good()) {
        stream >> tmp;
        if (trim(tmp)=="ROOT") {
            Joint* joint = new Joint();
            loadJoint(stream, body_root, joint);
        }
        else if(trim(tmp) == "MOTION")
            loadMotion(stream);
    }
}

void Tree::loadJoint(std::istream& stream, Node* parent, Joint* joint) {
    // load joint name
    std::string* name = new std::string;
    stream >> *name;
    joint->joint_name = name->c_str();
    
    std::string tmp;
    
    // setting local matrix to identity
    joint->parent_to_child << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    
    static int _channel_start = 0;
    unsigned channel_order_index = 0;
    
    
    BoxNode* child_node = new BoxNode(body_num, default_size, default_size, default_size);
    body_num ++;
    // TODO: setup the node
    child_node->setParent(parent, joint);
    
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
            Joint* child_joint = new Joint();
            loadJoint(stream, child_node, child_joint);
        }
        else if( tmp == "End" )
        {
            // loading End Site joint
            stream >> tmp >> tmp; // Site {
            
            Joint* tmp_joint = new Joint;
            
//            tmp_joint->parent = joint;
//            tmp_joint->num_channels = 0;
//            tmp_joint->name = "EndSite";
//            joint->children.push_back(tmp_joint);
//
//            stream >> tmp;
//            if( tmp == "OFFSET" )
//                stream >> tmp_joint->offset.x
//                >> tmp_joint->offset.y
//                >> tmp_joint->offset.z;
//
//            stream >> tmp;
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

void Tree::drawMyHuman(Node *node)
{
    node->draw();
    vector<Node*> children_vec = node->getChildren();
    for (size_t i=0; i<children_vec.size(); i++) {
        Joint *j = children_vec[i]->getJoint();
        glPushMatrix();
        j->transform();
        drawMyHuman(children_vec[i]);
        glPopMatrix();
    }
}

Node* Tree::getRoot()
{
    return body_root;
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
                loadHierarchy(file);
            break;
        }
        file.close();
    }
}
