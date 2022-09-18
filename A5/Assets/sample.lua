-- Create the top level puppet node named 'root'.
puppetNode = gr.node('puppet')
puppetNode:translate(0, 6.2, 0)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, {0 , 0, 0}, 0, 1, 0)
torsojoint = gr.joint('torsojoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0)
puppetNode:add_child(torsojoint)
white_skin_tone = gr.material({234.0/255.0, 192.0/255.0, 134.0/255.0}, {0.0, 0.0, 0.0}, 0.0, {0, 0, 0}, 0, 1, 0)
head_skin_tone = gr.material({234.0/255.0, 192.0/255.0, 134.0/255.0}, {0.5, 0.5, 0.5}, 20.0, {0, 0, 0}, 0, 1, 0)
glass = gr.material({0.0, 0.0, 0.0}, {0.7, 0.7, 0.7}, 25.0, {0.1, 0.1, 0.1}, 1, 1.54, 0)
mirror = gr.material({0, 0, 0}, {0, 0, 0}, 0, {1, 1, 1}, 0, 1, 0)
glossymirror = gr.material({0, 0, 0}, {0, 0, 0}, 0, {1, 1, 1}, 0, 1, 4)
water = gr.material({170 / 255, 213/255, 219/255}, {0.15, 0.15, 0.15}, 10.0, {0.15, 0.15, 0.15}, 1, 1.333, 0)
better_grass = gr.texture('grass.jpeg')
albania_flag = gr.texture('albania_flag.png')
red = gr.material({1.0, 0.0, 0.0}, {0.8, 0.8, 0.8}, 20, {0 , 0, 0}, 0, 1, 0)
blue = gr.material({0.0, 0.0, 1.0}, {0.8, 0.8, 0.8}, 20, {0 , 0, 0}, 0, 1, 0)
vertical_stripes = gr.texture('vertical_stripes.jpeg')
-- Create a GeometryNode with MeshId = 's1', and name = 'torso'.
-- MeshId's must reference a .obj file that has been previously loaded into
-- the MeshConsolidator instance within A3::init().
torsoMesh = gr.sphere('s1')
torsoMesh:scale(1.0, 1.5, 0.2)
torsoMesh:translate(0.0, -0.5, -5.0)
torsoMesh:set_material(white_skin_tone)
torsojoint:add_child(torsoMesh)


-- Create a shoulderMesh GeometryNode with MeshID = 's1' and name = 'shoulders'
shoulderMesh = gr.sphere('s1')
shoulderMesh:scale(1.3, 0.25, 0.2)
shoulderMesh:translate(0.0, 1.0, -5.0)
shoulderMesh:set_material(white_skin_tone)
torsoMesh:add_child(shoulderMesh)

neckJoint = gr.joint('neckjoint', {-45.0, 0.0, 45.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, -5.0}) -- TODO: change joint location
shoulderMesh:add_child(neckJoint)

neckMesh = gr.sphere('s1')
neckMesh:scale(0.3, 0.5, 0.2)
neckMesh:translate(0.0, 1.5, -5.0)
neckMesh:set_material(white_skin_tone)
neckJoint:add_child(neckMesh)
neckMesh:add_parent(neckJoint)


headJoint = gr.joint('headjoint', {-45.0, 0.0, 45.0}, {-45.0, 0.0, 45.0}, {0.0, 2.0, -5.0}) -- TODO: change joint location
neckMesh:add_child(headJoint)

-- Create a GeometryNode with MeshId = 's1', and name = 'head'.
headMesh = gr.sphere('s1')
headMesh:scale(0.5, 0.5, 0.4)
headMesh:translate(0.0, 2.5, -5.0)
headMesh:set_material(head_skin_tone)
headJoint:add_child(headMesh)
headMesh:add_parent(headJoint)

leftearMesh = gr.sphere('s1')
leftearMesh:scale(0.1, 0.1, 0.02)
leftearMesh:translate(-0.5, 2.5, -5.0)
leftearMesh:set_material(white_skin_tone)
headMesh:add_child(leftearMesh)

rightearMesh = gr.sphere('s1')
rightearMesh:scale(0.1, 0.1, 0.02)
rightearMesh:translate(0.5, 2.5, -5.0)
rightearMesh:set_material(white_skin_tone)
headMesh:add_child(rightearMesh)


leftshoulderjoint = gr.joint('leftshoulderjoint', {0.0, 89.9, 90.0}, {0.0, 0.0, 0.0}, {-1.3, 0.93, -5.0})
shoulderMesh:add_child(leftshoulderjoint)

leftupperarmMesh = gr.sphere('s1')
leftupperarmMesh:scale(0.25, 0.7, 0.2)
leftupperarmMesh:translate(-1.3, 0.23, -5.0)
leftupperarmMesh:set_material(white_skin_tone)
leftshoulderjoint:add_child(leftupperarmMesh)
leftupperarmMesh:add_parent(leftshoulderjoint)

leftelbowjoint = gr.joint('leftelbowjoint', {0.0, 89.9, 90.0}, {0.0, 0.0, 0.0}, {-1.3, -0.53, -5.0}) -- TODO: change
leftupperarmMesh:add_child(leftelbowjoint)

leftforearmMesh = gr.sphere('s1')
leftforearmMesh:scale(0.15, 0.7, 0.2)
leftforearmMesh:translate(-1.3, -1.2, -5.0)
leftforearmMesh:set_material(white_skin_tone)
leftelbowjoint:add_child(leftforearmMesh)
leftforearmMesh:add_parent(leftelbowjoint)

leftwristjoint = gr.joint('leftwristjoint', {-45.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {-1.3, -1.93, -5.0}) -- TODO:
leftforearmMesh:add_child(leftwristjoint)

lefthandMesh = gr.sphere('s1')
lefthandMesh:scale(0.2, 0.2, 0.2)
lefthandMesh:translate(-1.3, -2.1, -5.0)
lefthandMesh:set_material(white_skin_tone)
leftwristjoint:add_child(lefthandMesh)
lefthandMesh:add_parent(leftwristjoint)

hipsMesh = gr.sphere('s1')
hipsMesh:scale(0.7, 0.35, 0.2)
hipsMesh:translate(0.0, -2.1, -5.0)
hipsMesh:set_material(white_skin_tone)
torsoMesh:add_child(hipsMesh)

leftthighjoint = gr.joint('leftthighjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {0.5, -2.3, -5.0}) -- TODO:
hipsMesh:add_child(leftthighjoint)

leftthighMesh = gr.sphere('s1')
leftthighMesh:scale(0.35, 0.9, 0.2)
leftthighMesh:translate(-0.5, -3.2, -5.0)
leftthighMesh:set_material(white_skin_tone)
leftthighjoint:add_child(leftthighMesh)
leftthighMesh:add_parent(leftthighjoint)

leftkneejoint = gr.joint('leftkneejoint', {-90.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.5, -4.1, -5.0}) -- TODO:
leftthighMesh:add_child(leftkneejoint)

leftcalfMesh = gr.sphere('s1')
leftcalfMesh:scale(0.2, 0.85, 0.2)
leftcalfMesh:translate(-0.5, -4.95, -5.0)
leftcalfMesh:set_material(white_skin_tone)
leftkneejoint:add_child(leftcalfMesh)
leftcalfMesh:add_parent(leftkneejoint)

leftanklejoint = gr.joint('leftanklejoint', {-75.0, 0.0, 30.0}, {0.0, 0.0, 0.0}, {0.5, -5.8, -5.0}) -- TODO:
leftcalfMesh:add_child(leftanklejoint)

leftfootMesh = gr.sphere('s1')
leftfootMesh:scale(0.5, 0.2, 0.2)
leftfootMesh:translate(-0.95, -6.0, -5.0)
leftfootMesh:set_material(white_skin_tone)
leftanklejoint:add_child(leftfootMesh)
leftfootMesh:add_parent(leftanklejoint)

rightthighjoint = gr.joint('rightthighjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {0.5, -2.3, -5.0}) -- TODO:
hipsMesh:add_child(rightthighjoint)

rightthighMesh = gr.sphere('s1')
rightthighMesh:scale(0.35, 0.9, 0.2)
rightthighMesh:translate(0.5, -3.2, -5.0)
rightthighMesh:set_material(white_skin_tone)
rightthighjoint:add_child(rightthighMesh)
rightthighMesh:add_parent(rightthighjoint)

rightkneejoint = gr.joint('rightkneejoint', {-90.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.5, -4.1, -5.0}) -- TODO:
rightthighMesh:add_child(rightkneejoint)

rightcalfMesh = gr.sphere('s1')
rightcalfMesh:scale(0.2, 0.85, 0.2)
rightcalfMesh:translate(0.5, -4.95, -5.0)
rightcalfMesh:set_material(white_skin_tone)
rightkneejoint:add_child(rightcalfMesh)
rightcalfMesh:add_parent(rightkneejoint)

rightanklejoint = gr.joint('rightanklejoint', {-75.0, 0.0, 30.0}, {0.0, 0.0, 0.0}, {0.5, -5.8, -5.0}) --TODO:
rightcalfMesh:add_child(rightanklejoint)

rightfootMesh = gr.sphere('s1')
rightfootMesh:scale(0.5, 0.2, 0.2)
rightfootMesh:translate(0.95, -6.0, -5.0)
rightfootMesh:set_material(white_skin_tone)
rightanklejoint:add_child(rightfootMesh)
rightfootMesh:add_parent(rightanklejoint)


rightshoulderjoint = gr.joint('rightshoulderjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {1.3, 0.93, -5.0}) --TODO:
shoulderMesh:add_child(rightshoulderjoint)

rightupperarmMesh = gr.sphere('s1')
rightupperarmMesh:scale(0.25, 0.7, 0.2)
rightupperarmMesh:translate(1.3, 0.23, -5.0)
rightupperarmMesh:set_material(white_skin_tone)
rightshoulderjoint:add_child(rightupperarmMesh)
rightupperarmMesh:add_parent(rightshoulderjoint)

rightelbowjoint = gr.joint('rightelbowjoint', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {1.3, -0.53, -5.0}) -- TODO:
rightupperarmMesh:add_child(rightelbowjoint)

rightforearmMesh = gr.sphere('s1')
rightforearmMesh:scale(0.15, 0.7, 0.2)
rightforearmMesh:translate(1.3, -1.2, -5.0)
rightforearmMesh:set_material(white_skin_tone)
rightelbowjoint:add_child(rightforearmMesh)
rightforearmMesh:add_parent(rightelbowjoint)

rightwristjoint = gr.joint('rightwristjoint', {-45.0, 0.0, 90.0}, {0.0, 0.0, 0.0}, {1.3, -1.93, -5.0}) -- TODO:
rightforearmMesh:add_child(rightwristjoint)

righthandMesh = gr.sphere('s1')
righthandMesh:scale(0.2, 0.2, 0.2)
righthandMesh:translate(1.3, -2.1, -5.0)
righthandMesh:set_material(white_skin_tone)
rightwristjoint:add_child(righthandMesh)
righthandMesh:add_parent(rightwristjoint)

scene_root = gr.node('root')
--scene_root:add_child(puppetNode)
scene_root:translate(0, -2, 8)
scene_root:rotate('X', 23)
plane = gr.mesh('plane', 'plane.obj' )
for i = 1000, -1000, -1
do
	for j = 1000, -1000, -1
	do
		dcylinder = gr.cylinder('dc' .. tostring(i) .. ',' .. tostring(j))
		dcylinder:scale(0.001, gr.noise(1 / i, 1 / j, 4), 0.001)
		dcylinder:translate(i * 0.001, 0, j * 0.001)
		dcylinder:set_material(red)
		plane:add_child(dcylinder)
	end
end

scene_root:add_child(plane)
plane:set_material(better_grass)
plane:scale(75, 75, 75)


mirror1 = gr.mesh('mirror1', 'plane.obj')
mirror1:rotate('X', 90)
mirror1:rotate('Y', 60)
mirror1:scale(0.625, 1.875, 1)
mirror1:translate(-5, 0, -12)
mirror1:set_material(vertical_stripes)
--scene_root:add_child(mirror1)

transparent = gr.node('transparent')
c1 = gr.cylinder('c1')
c1:scale(0.1, 2, 0.1)
c1:set_material(blue)
transparent:add_child(c1)
c2 = gr.cone('c1')
c2:scale(0.5, 2, 1)
--c2:rotate('Z', 180.0)
c2:translate(0, 2, 0)
c2:set_material(red)
transparent:add_child(c2)
transparent:translate(0, 6, 3)
--scene_root:add_child(transparent)
white_light = gr.light({10, 15, 25}, {0.9, 0.9, 0.9}, {1, 0, 0}, 1, 1)
gr.render(scene_root, 'sample.png', 512, 512,
	  {0, 0, 25}, {0, 0, -5}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})