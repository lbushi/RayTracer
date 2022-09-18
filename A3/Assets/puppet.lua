-- Create the top level root node named 'root'.
rootNode = gr.node('root')

torsojoint = gr.joint('torsojoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rootNode:add_child(torsojoint)

-- Create a GeometryNode with MeshId = 'sphere', and name = 'torso'.
-- MeshId's must reference a .obj file that has been previously loaded into
-- the MeshConsolidator instance within A3::init().
torsoMesh = gr.mesh('sphere', 'torso')
torsoMesh:scale(1.0, 1.5, 0.2)
torsoMesh:translate(0.0, -0.5, -5.0)
torsoMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 10.0))
torsojoint:add_child(torsoMesh)


-- Create a shoulderMesh GeometryNode with MeshID = 'sphere' and name = 'shoulders'
shoulderMesh = gr.mesh('sphere', 'shoulders')
shoulderMesh:scale(1.3, 0.25, 0.2)
shoulderMesh:translate(0.0, 1.0, -5.0)
shoulderMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 10.0))
torsoMesh:add_child(shoulderMesh)

neckJoint = gr.joint('neckjoint', {-45.0, 0.0, 45.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, -5.0}) -- TODO: change joint location
shoulderMesh:add_child(neckJoint)

neckMesh = gr.mesh('sphere', 'neck')
neckMesh:scale(0.3, 0.5, 0.2)
neckMesh:translate(0.0, 1.5, -5.0)
neckMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
neckJoint:add_child(neckMesh)
neckMesh:add_parent(neckJoint)


headJoint = gr.joint('headjoint', {-45.0, 0.0, 45.0}, {-45.0, 0.0, 45.0}, {0.0, 2.0, -5.0}) -- TODO: change joint location
neckMesh:add_child(headJoint)

-- Create a GeometryNode with MeshId = 'sphere', and name = 'head'.
headMesh = gr.mesh('sphere', 'head')
headMesh:scale(0.5, 0.5, 0.4)
headMesh:translate(0.0, 2.5, -5.0)
headMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 50.0))
headJoint:add_child(headMesh)
headMesh:add_parent(headJoint)

leftearMesh = gr.mesh('sphere', 'leftear')
leftearMesh:scale(0.1, 0.1, 0.02)
leftearMesh:translate(-0.5, 2.5, -5.0)
leftearMesh:set_material(gr.material({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0.0))
headMesh:add_child(leftearMesh)

rightearMesh = gr.mesh('sphere', 'rightear')
rightearMesh:scale(0.1, 0.1, 0.02)
rightearMesh:translate(0.5, 2.5, -5.0)
rightearMesh:set_material(gr.material({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0.0))
headMesh:add_child(rightearMesh)


leftshoulderjoint = gr.joint('leftshoulderjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {-1.3, 0.93, -5.0})
shoulderMesh:add_child(leftshoulderjoint)

leftupperarmMesh = gr.mesh('sphere', 'leftupperarm')
leftupperarmMesh:scale(0.25, 0.7, 0.2)
leftupperarmMesh:translate(-1.3, 0.23, -5.0)
leftupperarmMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
leftshoulderjoint:add_child(leftupperarmMesh)
leftupperarmMesh:add_parent(leftshoulderjoint)

leftelbowjoint = gr.joint('leftelbowjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {-1.3, -0.53, -5.0}) -- TODO: change
leftupperarmMesh:add_child(leftelbowjoint)

leftforearmMesh = gr.mesh('sphere', 'leftforearm')
leftforearmMesh:scale(0.15, 0.7, 0.2)
leftforearmMesh:translate(-1.3, -1.2, -5.0)
leftforearmMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
leftelbowjoint:add_child(leftforearmMesh)
leftforearmMesh:add_parent(leftelbowjoint)

leftwristjoint = gr.joint('leftwristjoint', {-45.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {-1.3, -1.93, -5.0}) -- TODO:
leftforearmMesh:add_child(leftwristjoint)

lefthandMesh = gr.mesh('sphere', 'lefthand')
lefthandMesh:scale(0.2, 0.2, 0.2)
lefthandMesh:translate(-1.3, -2.1, -5.0)
lefthandMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
leftwristjoint:add_child(lefthandMesh)
lefthandMesh:add_parent(leftwristjoint)

hipsMesh = gr.mesh('sphere', 'hips')
hipsMesh:scale(0.7, 0.35, 0.2)
hipsMesh:translate(0.0, -2.1, -5.0)
hipsMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
torsoMesh:add_child(hipsMesh)

leftthighjoint = gr.joint('leftthighjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {0.5, -2.3, -5.0}) -- TODO:
hipsMesh:add_child(leftthighjoint)

leftthighMesh = gr.mesh('sphere', 'leftthigh')
leftthighMesh:scale(0.35, 0.9, 0.2)
leftthighMesh:translate(-0.5, -3.2, -5.0)
leftthighMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
leftthighjoint:add_child(leftthighMesh)
leftthighMesh:add_parent(leftthighjoint)

leftkneejoint = gr.joint('leftkneejoint', {-90.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.5, -4.1, -5.0}) -- TODO:
leftthighMesh:add_child(leftkneejoint)

leftcalfMesh = gr.mesh('sphere', 'leftcalf')
leftcalfMesh:scale(0.2, 0.85, 0.2)
leftcalfMesh:translate(-0.5, -4.95, -5.0)
leftcalfMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
leftkneejoint:add_child(leftcalfMesh)
leftcalfMesh:add_parent(leftkneejoint)

leftanklejoint = gr.joint('leftanklejoint', {-75.0, 0.0, 30.0}, {0.0, 0.0, 0.0}, {0.5, -5.8, -5.0}) -- TODO:
leftcalfMesh:add_child(leftanklejoint)

leftfootMesh = gr.mesh('sphere', 'leftankle')
leftfootMesh:scale(0.5, 0.2, 0.2)
leftfootMesh:translate(-0.95, -6.0, -5.0)
leftfootMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
leftanklejoint:add_child(leftfootMesh)
leftfootMesh:add_parent(leftanklejoint)

rightthighjoint = gr.joint('rightthighjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {0.5, -2.3, -5.0}) -- TODO:
hipsMesh:add_child(rightthighjoint)

rightthighMesh = gr.mesh('sphere', 'rightthigh')
rightthighMesh:scale(0.35, 0.9, 0.2)
rightthighMesh:translate(0.5, -3.2, -5.0)
rightthighMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
rightthighjoint:add_child(rightthighMesh)
rightthighMesh:add_parent(rightthighjoint)

rightkneejoint = gr.joint('rightkneejoint', {-90.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.5, -4.1, -5.0}) -- TODO:
rightthighMesh:add_child(rightkneejoint)

rightcalfMesh = gr.mesh('sphere', 'rightcalf')
rightcalfMesh:scale(0.2, 0.85, 0.2)
rightcalfMesh:translate(0.5, -4.95, -5.0)
rightcalfMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
rightkneejoint:add_child(rightcalfMesh)
rightcalfMesh:add_parent(rightkneejoint)

rightanklejoint = gr.joint('rightanklejoint', {-75.0, 0.0, 30.0}, {0.0, 0.0, 0.0}, {0.5, -5.8, -5.0}) --TODO:
rightcalfMesh:add_child(rightanklejoint)

rightfootMesh = gr.mesh('sphere', 'rightfoot')
rightfootMesh:scale(0.5, 0.2, 0.2)
rightfootMesh:translate(0.95, -6.0, -5.0)
rightfootMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
rightanklejoint:add_child(rightfootMesh)
rightfootMesh:add_parent(rightanklejoint)


rightshoulderjoint = gr.joint('rightshoulderjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {1.3, 0.93, -5.0}) --TODO:
shoulderMesh:add_child(rightshoulderjoint)

rightupperarmMesh = gr.mesh('sphere', 'rightupperarm')
rightupperarmMesh:scale(0.25, 0.7, 0.2)
rightupperarmMesh:translate(1.3, 0.23, -5.0)
rightupperarmMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
rightshoulderjoint:add_child(rightupperarmMesh)
rightupperarmMesh:add_parent(rightshoulderjoint)

rightelbowjoint = gr.joint('rightelbowjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {1.3, -0.53, -5.0}) -- TODO:
rightupperarmMesh:add_child(rightelbowjoint)

rightforearmMesh = gr.mesh('sphere', 'rightforearm')
rightforearmMesh:scale(0.15, 0.7, 0.2)
rightforearmMesh:translate(1.3, -1.2, -5.0)
rightforearmMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
rightelbowjoint:add_child(rightforearmMesh)
rightforearmMesh:add_parent(rightelbowjoint)

rightwristjoint = gr.joint('rightwristjoint', {-45.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {1.3, -1.93, -5.0}) -- TODO:
rightforearmMesh:add_child(rightwristjoint)

righthandMesh = gr.mesh('sphere', 'righthand')
righthandMesh:scale(0.2, 0.2, 0.2)
righthandMesh:translate(1.3, -2.1, -5.0)
righthandMesh:set_material(gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 5.0))
rightwristjoint:add_child(righthandMesh)
righthandMesh:add_parent(rightwristjoint)

-- Return the root with all of it's childern.  The SceneNode A3::m_rootNode will be set
-- equal to the return value from this Lua script.
return rootNode
