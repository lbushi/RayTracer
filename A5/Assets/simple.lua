-- A simple scene with five spheres

reflective_mat = gr.material({0.53, 0.53, 0.53}, {0.5, 0.7, 0.5}, 25, {0.8, 0.8, 0.8}, 0, 1, 0)
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, {0, 0, 0}, 0, 1, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, {0, 0, 0}, 0, 1, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, {0, 0, 0}, 0, 1, 0)
emerald = gr.material({0.07568, 0.61424, 0.07568}, {0.633, 0.727811, 0.633} ,76.8, {0.55, 0.55, 0.55}, 1, 1.6, 0)

 world = gr.texture('world.jpeg')
scene_root = gr.node('root')
s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
--scene_root:add_child(s1)
s1:set_material(mat1)



dodecahedron = gr.mesh('d1', 'dodeca.obj')
dodecahedron:scale(100, 100, 100)
dodecahedron:translate(0, 20, -500)
dodecahedron:set_material(emerald)
scene_root:add_child(dodecahedron)

s2 = gr.sphere('s2', {200, 50, -100}, 150)
s2:scale(270, 270, 270)
s2:rotate('Y', 180)
s2:rotate('X', 45)
s2:translate(0, 50, -100)
--scene_root:add_child(s2)
s2:set_material(world)

s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
--scene_root:add_child(s3)
s3:set_material(mat2)


s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
--scene_root:add_child(s4)
s4:set_material(mat3)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
--scene_root:add_child(s5)
s5:set_material(mat1)

white_light = gr.light({0.0, 0.0, 100.0}, {0.9, 0.9, 0.9}, {1, 0, 0}, 0, 0)
--magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0}, 0, 0)

gr.render(scene_root, 'simple.png', 512, 512,
	  {0, 0, 800}, {0, 0, -400}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
