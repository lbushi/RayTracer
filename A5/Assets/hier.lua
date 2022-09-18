-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, {0 ,0, 0}, 0, 0, 0)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0, {0 ,0, 0}, 0, 0, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25, {0 ,0, 0}, 0, 0,0, 0)
emerald = gr.material({0.07568, 0.61424, 0.07568}, {0.633, 0.727811, 0.633} ,76.8, {0.55, 0.55, 0.55}, 1, 1.6, 0)
scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -12)

-- the arc
arc = gr.node('arc')
scene:add_child(arc)
arc:translate(0,0,-10)
arc:rotate('Y', 60)
p1 = gr.cube('p1')
arc:add_child(p1)
p1:set_material(gold)
p1:scale(0.8, 4, 0.8)
p1:translate(-2.4, 0, -0.4)

p2 = gr.cube('p2')
arc:add_child(p2)
p2:set_material(gold)
p2:scale(0.8, 4, 0.8)
p2:translate(1.6, 0, -0.4)

s = gr.sphere('s')
arc:add_child(s)
s:set_material(gold)
s:scale(4, 0.6, 0.6)
s:translate(0, 4, 0)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

-- sphere
poly = gr.mesh( 'poly', 'dodeca.obj' )
scene:add_child(poly)
poly:translate(-7, 1.618034, 0)
poly:set_material(emerald)


l1 = gr.light({200, 200, 400}, {0.8, 0.8, 0.8}, {1, 0, 0}, 0, 0)
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0}, 0, 0)

gr.render(scene, 'hier.png', 256, 256, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.1, 0.1, 0.1}, {l1, l2})
