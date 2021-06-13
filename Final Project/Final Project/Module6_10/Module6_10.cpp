//============================================================================
//	Johns Hopkins University Engineering Programs for Professionals
//	605.467 Computer Graphics and 605.767 Applied Computer Graphics
//	Instructor:	David W. Nesbitt
//
//	Author:  David W. Nesbitt
//	File:    Module6_10.cpp
//	Purpose: OpenGL and GLUT program to draw a simple 3-D scene. It starts
//           with some simple object modeling and representation, adds camera
//           and projection controls, adds lighting and shading, then adds
//           texture mapping.
//
//============================================================================

#include <iostream>
#include <vector>

#include <GL/gl3w.h>
#include <GL/freeglut.h>
#include "geometry/geometry.h"
#include "shader_support/glsl_shader.h"
#include "scene/scene.h"
#include "ParticleShaderNode.h"
#include "ParticleEmitter.h"
// NOTE - moved object geometry nodes to scene directory
#include "lighting_shader_node.h"
#include "ObjGeometry.h"
#include "objloader.h"
#include "skybox_shader_node.h"
#include "SkyboxGeometry.h"
#include "SkyboxPresentation.h"
#include "BumpMappedSquare.h"
#include "PyramidGeometry.h"

// Root of the scene graph and scene state
SceneNode* SceneRoot;
SceneState MySceneState;

// Global camera node (so we can change view)
CameraNode* MyCamera;

// Keep the spotlight global so we can update its poisition
LightNode* Spotlight;

ParticleEmitter* particleEmitter;

// While mouse button is down, the view will be updated
bool  Animate = false;
bool  Forward = true;
float Velocity = 1.0f;
int   MouseX;
int   MouseY;
int   RenderWidth  = 800;
int   RenderHeight = 600;
const float FrameRate = 72.0f;

// Simple logging function
void logmsg(const char *message, ...)
{
   // Open file if not already opened
   static FILE *lfile = NULL;
   if (lfile == NULL)
      lfile = fopen("Module6_10.log", "w");

   va_list arg;
   va_start(arg, message);
   vfprintf(lfile, message, arg);
   putc('\n', lfile);
   fflush(lfile);
   va_end(arg);
}

// Update the spotlight based on camera position change
void UpdateSpotlight() {
  Point3 pos = MyCamera->GetPosition();
  Spotlight->SetPosition(HPoint3(pos.x, pos.y, pos.z, 1.0f));
  Vector3 dir = MyCamera->GetViewPlaneNormal() * -1.0f;
  Spotlight->SetSpotlightDirection(dir);
}

/**
 * Updates the view given the mouse position and whether to move
 * forward or backward.
 * @param  x        Window x position.
 * @param  y        Window y position.
 * @param  forward  Forward flag (true if moving forward).
 */
void UpdateView(const int x, const int y, bool forward) {
  // Find relative dx and dy relative to center of the window
  float dx = 4.0f * ((x - (static_cast<float>(RenderWidth * 0.5f))) /
    static_cast<float>(RenderWidth));
  float dy = 4.0f * (((static_cast<float>(RenderHeight * 0.5f) - y)) /
    static_cast<float>(RenderHeight));
  float dz = (forward) ? Velocity : -Velocity;
  MyCamera->MoveAndTurn(dx * Velocity, dy * Velocity, dz);
  UpdateSpotlight();
  glutPostRedisplay();
}

/**
 * Display callback. Clears the prior scene and draws a new one.
 */
void display() {
  // Clear the framebuffer and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Init scene state and draw the scene graph
  MySceneState.Init();
  SceneRoot->Draw(MySceneState);

  int error = glGetError();
  // Swap buffers
  glutSwapBuffers();

   error = glGetError();
}

/**
 * Use a timer method to try to do a consistent update rate or 72Hz.
 * Without using a timer, the speed of movement will depend on how fast
 * the program runs (fast movement on a fast PC and slow movement on a
 * slower PC)
 */
void timerFunction(int value) {
  // If mouse button is down, generate another view
  if (Animate) {
    UpdateView(MouseX, MouseY, Forward);
    glutTimerFunc((int)(1000.0f / FrameRate), timerFunction, 0);
  }
}

/**
 * Mouse callback (called when a mouse button state changes)
 */
void mouse(int button, int state, int x, int y) {
  // On a left button up event MoveAndTurn the view with forward motion
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      MouseX = x;
      MouseY = y;
      Forward = true;
      Animate = true;
      UpdateView(x, y, true);

      // Set update
      glutTimerFunc((int)(1000.0f / FrameRate), timerFunction, 0);
    }
    else {
      Animate = false;  // Disable animation when the button is released
    }
  }

  // On a right button up event MoveAndTurn the view with reverse motion
  if (button == GLUT_RIGHT_BUTTON) {
    if (state == GLUT_DOWN) {
      MouseX = x;
      MouseY = y;
      Forward = false;
      Animate = true;
      UpdateView(x, y, true);

      // Set update
      glutTimerFunc((int)(1000.0f / FrameRate), timerFunction, 0);
    }
    else {
      Animate = false;  // Disable animation when the button is released
    }
  }
}

/**
 * Mouse motion callback (called when mouse button is depressed)
 */
void mouseMotion(int x, int y) {
  // Update position used for changing the view and force a new view
  MouseX = x;
  MouseY = y;
  UpdateView(x, y, true);
}

/**
 * Keyboard callback.
 */
void keyboard(unsigned char key, int x, int y) {
  switch (key) {
   // Escape key
   case 27:
    exit(0);
    break;

   // Reset the view
   case 'i':
    MyCamera->SetPosition(Point3(0.0f, -100.0f, 20.0f));
    MyCamera->SetLookAtPt(Point3(0.0f, 0.0f, 20.0f));
    MyCamera->SetViewUp(Vector3(0.0, 0.0, 1.0));
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Roll the camera by 5 degrees
   case 'r':
    MyCamera->Roll(5);
    glutPostRedisplay();
    break;

   // Roll the camera by 5 degrees (clockwise)
   case 'R':
    MyCamera->Roll(-5);
    glutPostRedisplay();
    break;

   // Change the pitch of the camera by 5 degrees
   case 'p':
    MyCamera->Pitch(5);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Change the pitch of the camera by 5 degrees (clockwise)
   case 'P':
    MyCamera->Pitch(-5);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Change the heading of the camera by 5 degrees
   case 'h':
    MyCamera->Heading(5);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Change the heading of the camera by 5 degrees (clockwise)
   case 'H':
    MyCamera->Heading(-5);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Go faster
   case 'V':
    Velocity += 0.2f;
    break;

   // Go slower
   case 'v':
    Velocity -= 0.2f;
    if (Velocity < 0.2f)
      Velocity = 0.1f;
    break;

   // Slide right
   case 'X':
    MyCamera->Slide(5.0f, 0.0f, 0.0f);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Slide left
   case 'x':
    MyCamera->Slide(-5.0f, 0.0f, 0.0f);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Slide up
   case 'Y':
    MyCamera->Slide(0.0f, 5.0f, 0.0f);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Slide down
   case 'y':
    MyCamera->Slide(0.0f, -5.0f, 0.0f);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Move forward
   case 'F':
    MyCamera->Slide(0.0f, 0.0f, -5.0f);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

   // Move backward
   case 'f':
    MyCamera->Slide(0.0f, 0.0f, 5.0f);
    UpdateSpotlight();
    glutPostRedisplay();
    break;

  default:
    break;
  }
}

/**
 * Reshape callback. Update projection to reflect new aspect ratio.
 * @param  width  Window width
 * @param  height Window height
 */
void reshape(int width, int height) {
  RenderWidth  = width;
  RenderHeight = height;

  // Reset the viewport
  glViewport(0, 0, width, height);

  // Reset the perspective projection to reflect the change of aspect ratio 
  // Make sure we cast to float so we get a fractional aspect ratio.
  MyCamera->ChangeAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}

/**
 * Convenience method to add a material, then a transform, then a 
 * geometry node as a child to a specified parent node.
 * @param  parent    Parent scene node.
 * @param  material  Presentation node.
 * @param  transform Transformation node.
 * @param  geometry  Geometry node.
 */
void AddSubTree(SceneNode* parent, SceneNode* material,
                SceneNode* transform, SceneNode* geometry) {
  parent->AddChild(material);
  material->AddChild(transform);
  transform->AddChild(geometry);
}

/**
 * Construct room as a child of the specified node
 * @param  parent       Parent node
 * @param  unit_square  Geometry node to use
 * @return Returns a scene node that describes the room.
 */
SceneNode* ConstructRoom(UnitSquareSurface* unit_square, 
        TexturedUnitSquareSurface* textured_square) {
  // Contruct transform nodes for the walls. Perform rotations so the 
  // walls face inwards
  TransformNode* floor_transform = new TransformNode;
  floor_transform->Scale(200.0f, 200.0f, 1.0f);

  // Back wall is rotated +90 degrees about x: (y -> z)
  TransformNode* backwall_transform = new TransformNode;
  backwall_transform->Translate(0.0f, 100.0f, 40.0f);
  backwall_transform->RotateX(90.0f);
  backwall_transform->Scale(200.0f, 80.0f, 1.0f);

  // Front wall is rotated -90 degrees about x: (z -> y)
  TransformNode* frontwall_transform = new TransformNode;
  frontwall_transform->Translate(0.0f, -100.0f, 40.0f);
  frontwall_transform->RotateX(-90.0f);
  frontwall_transform->Scale(200.0f, 80.0f, 1.0f);

  // Left wall is rotated 90 degrees about y: (z -> x)
  TransformNode* leftwall_transform = new TransformNode;
  leftwall_transform->Translate(-100.0f, 0.0f, 40.0f);
  leftwall_transform->RotateY(90.0f);
  leftwall_transform->Scale(80.0f, 200.0f, 1.0f);

  // Right wall is rotated -90 about y: (z -> -x)
  TransformNode* rightwall_transform = new TransformNode;
  rightwall_transform->Translate(100.0f, 0.0f, 40.0f);
  rightwall_transform->RotateY(-90.0f);
  rightwall_transform->Scale(80.0f, 200.0f, 1.0f);

  // Ceiling is rotated 180 about x so it faces inwards
  TransformNode* ceiling_transform = new TransformNode;
  ceiling_transform->Translate(0.0f, 0.0f, 80.0f);
  ceiling_transform->RotateX(180.0f);
  ceiling_transform->Scale(200.0f, 200.0f, 1.0f);

  // Use a texture for the floor
  PresentationNode* floor_material = new PresentationNode(Color4(0.15f, 0.15f, 0.15f),
        Color4(0.4f, 0.4f, 0.4f), Color4(0.2f, 0.2f, 0.2f), Color4(0.0f, 0.0f, 0.0f), 5.0f);
  floor_material->SetTexture("floor_tiles.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

  // Make the walls reddish, slightly shiny
  PresentationNode* wall_material = new PresentationNode(Color4(0.35f, 0.225f, 0.275f),
    Color4(0.7f, 0.55f, 0.55f), Color4(0.4f, 0.4f, 0.4f), Color4(0.0f, 0.0f, 0.0f), 16.0f);

  // Ceiling should be white, moderately shiny
  PresentationNode* ceiling_material = new PresentationNode(Color4(0.75f, 0.75f, 0.75f),
    Color4(1.0f, 1.0f, 1.0f), Color4(0.9f, 0.9f, 0.9f), Color4(0.0f, 0.0f, 0.0f), 64.0);

  // Walls. We can group these all under a single presentation node.
  SceneNode* room = new SceneNode;
  room->AddChild(wall_material);
  wall_material->AddChild(backwall_transform);
  backwall_transform->AddChild(unit_square);
  wall_material->AddChild(leftwall_transform);
  leftwall_transform->AddChild(unit_square);
  wall_material->AddChild(rightwall_transform);
  rightwall_transform->AddChild(unit_square);
  wall_material->AddChild(frontwall_transform);
  frontwall_transform->AddChild(unit_square);

  // Add floor and ceiling to the parent. Use convenience method to add
  // presentation, then transform, then geometry.
  AddSubTree(room, floor_material, floor_transform, textured_square);
  AddSubTree(room, ceiling_material, ceiling_transform, unit_square);

  return room;
}

/**
 * Construct table
 * @param  unit_square  Geometry node to use for table top
 * @param  legs        Geometry node to use for legs
 * @return Returns a scene node representing the table
 */
SceneNode* ConstructTable(SceneNode* box, ConicSurface* leg) {
  // Table legs (relative to center of table)
  TransformNode* lfleg_transform = new TransformNode;
  lfleg_transform->Translate(-20.0f, -10.0f, 10.0f);
  lfleg_transform->Scale(6.0f, 6.0f, 20.0f);
  TransformNode* lrleg_transform = new TransformNode;
  lrleg_transform->Translate(-20.0f, 10.0f, 10.0f);
  lrleg_transform->Scale(6.0f, 6.0f, 20.0f);
  TransformNode* rfleg_transform = new TransformNode;
  rfleg_transform->Translate(20.0f, -10.0f, 10.0f);
  rfleg_transform->Scale(6.0f, 6.0f, 20.0f);
  TransformNode* rrleg_transform = new TransformNode;
  rrleg_transform->Translate(20.0f, 10.0f, 10.0f);
  rrleg_transform->Scale(6.0f, 6.0f, 20.0f);

  // Construct dimensions for the table top
  TransformNode* top_transform = new TransformNode;
  top_transform->Translate(0.0f, 0.0f, 23.0f);
  top_transform->Scale(60.0f, 30.0f, 6.0f);

  // Create the tree
  SceneNode* table = new SceneNode;
  table->AddChild(top_transform);
  top_transform->AddChild(box);
  table->AddChild(lfleg_transform);
  lfleg_transform->AddChild(leg);
  table->AddChild(rfleg_transform);
  rfleg_transform->AddChild(leg);
  table->AddChild(lrleg_transform);
  lrleg_transform->AddChild(leg);
  table->AddChild(rrleg_transform);
  rrleg_transform->AddChild(leg);

  return table;
}

/**
 * Construct a unit box with outward facing normals.
 * @param  unit_square  Geometry node to use
 */
SceneNode* ConstructUnitBox(UnitSquareSurface* unit_square) {
  // Contruct transform nodes for the sides of the box.
  // Perform rotations so the sides face outwards

  // Bottom is rotated 180 degrees so it faces outwards
  TransformNode* bottom_transform = new TransformNode;
  bottom_transform->Translate(0.0f, 0.0f, -0.5f);
  bottom_transform->RotateX(180.0f);

  // Back is rotated -90 degrees about x: (z -> y)
  TransformNode* back_transform = new TransformNode;
  back_transform->Translate(0.0f, 0.5f, 0.0f);
  back_transform->RotateX(-90.0f);

  // Front wall is rotated 90 degrees about x: (y -> z)
  TransformNode* front_transform = new TransformNode;
  front_transform->Translate(0.0f, -0.5f, 0.0f);
  front_transform->RotateX(90.0f);

  // Left wall is rotated -90 about y: (z -> -x)
  TransformNode* left_transform = new TransformNode;
  left_transform->Translate(-0.5f, 0.0f, 00.0f);
  left_transform->RotateY(-90.0f);

  // Right wall is rotated 90 degrees about y: (z -> x)
  TransformNode* right_transform = new TransformNode;
  right_transform->Translate(0.5f, 0.0f, 0.0f);
  right_transform->RotateY(90.0f);

  // Top 
  TransformNode* top_transform = new TransformNode;
  top_transform->Translate(0.0f, 0.0f, 0.50f);

  // Create a SceneNode and add the 6 sides of the box.
  SceneNode* box = new SceneNode;
  box->AddChild(back_transform);
  back_transform->AddChild(unit_square);
  box->AddChild(left_transform);
  left_transform->AddChild(unit_square);
  box->AddChild(right_transform);
  right_transform->AddChild(unit_square);
  box->AddChild(front_transform);
  front_transform->AddChild(unit_square);
  box->AddChild(bottom_transform);
  bottom_transform->AddChild(unit_square);
  box->AddChild(top_transform);
  top_transform->AddChild(unit_square);

  return box;
}

/** 
 * Construct vase using a surface of revolution.
 */
SceneNode* ConstructVase(const int position_loc, const int normal_loc) {
  // Profile curve. Unit width and height, centered at the center of the vase
  std::vector<Point3> v = {
    { 0.0f, 0.0f, -0.5f },
    { 0.4f, 0.0f, -0.5f },
    { 0.6f, 0.0f, -0.45f },
    { 0.72f, 0.0f, -0.37f },
    { 0.81f, 0.0f, -0.26f },
    { 0.82f, 0.0f, -0.18f },
    { 0.79f, 0.0f, -0.08f },
    { 0.7f, 0.0f, 0.02f },
    { 0.55f, 0.0f, 0.13f },
    { 0.48f, 0.0f, 0.25f },
    { 0.51f, 0.0f, 0.35f },
    { 0.53f, 0.0f, 0.41f },
    { 0.62f, 0.0f, 0.45f },
    { 0.62f, 0.0f, 0.5f },
    { 0.65f, 0.0f, 0.5f },
    { 0.0f,  0.0f, 0.5f } };
  SurfaceOfRevolution* surf = new SurfaceOfRevolution(v, 36, position_loc,
    normal_loc);

  // Vase color and position
  PresentationNode* vase_material = new PresentationNode(Color4(0.35f, 0.15f, 0.25f),
        Color4(0.95f, 0.35f, 0.65f), Color4(0.4f, 0.4f, 0.4f),
        Color4(0.0f, 0.0f, 0.0f), 16.0f);
  TransformNode* vase_transform = new TransformNode();
  vase_transform->Translate(0.0f, 75.0f, 10.0f);
  vase_transform->Scale(10.0f, 10.0f, 20.0f);

  // Form scene graph
  SceneNode* vase = new SceneNode();
  vase->AddChild(vase_material);
  vase->AddChild(vase_transform);
  vase->AddChild(surf);
  return vase;
}

/**
 * Construct a sphere with a shiny blue material.
 */
SceneNode* ConstructGlobe(const int position_loc, const int normal_loc,
              const int texture_loc) {
  TexturedSphereSection* sphere = new TexturedSphereSection(-90.0f, 90.0f, 18, -180.0f, 180.0f,
          36, 1.0f, position_loc, normal_loc, texture_loc);

  // Globe material
  PresentationNode* globe_material = new PresentationNode(
    Color4(0.15f, 0.15f, 0.15f), Color4(0.5f, 0.5f, 0.5f),
    Color4(1.0f, 1.0f, 1.0f), Color4(0.0f, 0.0f, 0.0f), 85.0f);
  globe_material->SetTexture("Penguin_Diffuse_Color.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

  // Sphere
  TransformNode* sphere_transform = new TransformNode;
  sphere_transform->Translate(80.0f, 20.0f, 10.0f);
  sphere_transform->Scale(10.0f, 10.0f, 10.0f);

  // Form scene graph
  SceneNode* globe = new SceneNode();
  AddSubTree(globe, globe_material, sphere_transform, sphere);
  return globe;
}

/**
 * Construct a simple painting on the back wall
 */
SceneNode* ConstructPainting(const int position_loc, const int normal_loc,
                          const int texture_loc) {
  TexturedUnitSquareSurface* textured_square = new TexturedUnitSquareSurface(2, 1, position_loc,
            normal_loc, texture_loc);

  // Mmaterial
  PresentationNode* picture_material = new PresentationNode(
    Color4(0.5f, 0.5f, 0.5f), Color4(0.75f, 0.75f, 0.75f),
    Color4(0.1f, 0.1f, 0.1f), Color4(0.0f, 0.0f, 0.0f), 5.0f);
  picture_material->SetTexture("dogs_poker.jpg", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
          GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

  // Transform
  TransformNode* painting_transform = new TransformNode;
  painting_transform->Translate(50.0f, 99.0f, 40.0f);
  painting_transform->RotateX(90.0f);
  painting_transform->Scale(30.0f, 22.0f, 1.0f);

  // Form scene graph
  SceneNode* picture = new SceneNode();
  AddSubTree(picture, picture_material, painting_transform, textured_square);
  return picture;
}

/**
* Construct a sphere with a shiny blue material.
*/
SceneNode* ConstructShinyTorus(const int position_loc, const int normal_loc) {
  TorusSurface* torus = new TorusSurface(20.0f, 5.0f, 36, 18, position_loc, normal_loc);

  // Shiny black
  PresentationNode* shiny_black = new PresentationNode(
    Color4(0.0f, 0.0f, 0.0f), Color4(0.01f, 0.01f, 0.01f),
    Color4(0.5f, 0.5f, 0.5f), Color4(0.0f, 0.0f, 0.0f), 32.0f);

  // Torus transform
  TransformNode* torus_transform = new TransformNode;
  torus_transform->Translate(0.0f, 95.0f, 23.0f);
  torus_transform->RotateX(70.0f);

  // Form scene graph
  SceneNode* shiny_torus = new SceneNode();
  AddSubTree(shiny_torus, shiny_black, torus_transform, torus);
  return shiny_torus;
}


/**
 * Construct lighting for this scene. Note that it is hard coded
 * into the shader node for this exercise.
 * @param  lighting  Pointer to the lighting shader node.
 */
void ConstructLighting(LightingShaderNode* lighting) {
  // Set the global light ambient
  Color4 globalAmbient(0.4f, 0.4f, 0.4f, 1.0f);
  lighting->SetGlobalAmbient(globalAmbient);

  // Light 0 - point light source in back right corner
  LightNode* light0 = new LightNode(0);
  light0->SetDiffuse(Color4(0.5f, 0.5f, 0.5f, 1.0f));
  light0->SetSpecular(Color4(0.5f, 0.5f, 0.5f, 1.0f));
  light0->SetPosition(HPoint3(90.0f, 90.0f, 30.f, 1.0f));
  light0->Enable();

  // Light1 - directional light from the ceiling
  LightNode* light1 = new LightNode(1);
  light1->SetDiffuse(Color4(0.7f, 0.7f, 0.7f, 1.0f));
  light1->SetSpecular(Color4(0.7f, 0.7f, 0.7f, 1.0f));
  light1->SetPosition(HPoint3(0.0f, 0.0f, 1.0f, 0.0f));
  light1->Enable();

  // Spotlight - reddish spotlight - we will place at the camera location
  // shining along -VPN
  Spotlight = new LightNode(2);
  Spotlight->SetDiffuse(Color4(0.5f, 0.1f, 0.1f, 1.0f));
  Spotlight->SetSpecular(Color4(0.5f, 0.1f, 0.1f, 1.0f));
  Point3 pos = MyCamera->GetPosition();
  Spotlight->SetPosition(HPoint3(pos.x, pos.y, pos.z, 1.0f));
  Vector3 dir = MyCamera->GetViewPlaneNormal() * -1.0f;
  Spotlight->SetSpotlight(dir, 32.0f, 30.0f);
  Spotlight->Enable();

  // Lights are children of the camera node
  //MyCamera->AddChild(light0);
  lighting->AddChild(light0);
  light0->AddChild(light1);
  light1->AddChild(Spotlight);
}



void particleTimer(int value) {



	
	// Update the scene graph
	SceneState scene_state;
	SceneRoot->Update(scene_state);

	// Set update to specified frames per second
	glutTimerFunc((int)(1000.0f / 30), particleTimer, 0);

	glutPostRedisplay();
}


void createEgyptianSculture(SceneNode* myscene, ObjGeometry *shape)
{


	// Use a texture for the floor
	PresentationNode* sculptureMaterial = new PresentationNode(Color4(0.15f, 0.15f, 0.15f),
		Color4(0.4f, 0.4f, 0.4f), Color4(0.2f, 0.2f, 0.2f), Color4(0.14f, 0.0f, 0.0f), 5.0f);
	sculptureMaterial->SetTexture("Anubis_albedo.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);

	// Transform
	TransformNode* sculptureTranform = new TransformNode();
	sculptureTranform->Translate(0.0f, 0.0f, 0.0f);
	//penguinTranform->Translate(0.0f, 10.0f, 15.0f);
	sculptureTranform->RotateX(90);
	sculptureTranform->Scale(0.5f, 0.5f, 0.5f);

	
	AddSubTree(myscene, sculptureMaterial, sculptureTranform, shape);
}


void createPenguin(SceneNode* myscene, ObjGeometry *shape)
{


	// Use a texture for the floor
	PresentationNode* penguinMaterial = new PresentationNode(Color4(0.15f, 0.15f, 0.15f),
		Color4(0.4f, 0.4f, 0.4f), Color4(0.2f, 0.2f, 0.2f), Color4(0.14f, 0.0f, 0.0f), 5.0f);
	penguinMaterial->SetTexture("Penguin_Diffuse_Color.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);

	// Transform
	TransformNode* penguinTranform = new TransformNode();
	penguinTranform->Translate(-60.0f, 20.0f, 0.0f);
	//penguinTranform->Translate(0.0f, 10.0f, 15.0f);
	penguinTranform->RotateX(90);
	penguinTranform->RotateY(45);
	penguinTranform->Scale(10.0f, 10.0f, 10.0f);


	AddSubTree(myscene, penguinMaterial, penguinTranform, shape);
}

void createSphinx(SceneNode* myscene, ObjGeometry *shape)
{


	// Use a texture for the floor
	PresentationNode* sphinxMaterial = new PresentationNode(Color4(0.15f, 0.15f, 0.15f),
		Color4(0.4f, 0.4f, 0.4f), Color4(0.2f, 0.2f, 0.2f), Color4(0.14f, 0.0f, 0.0f), 5.0f);
	sphinxMaterial->SetTexture("Sphinx1_albedo.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);

	// Transform
	TransformNode* penguinTranform = new TransformNode();
	penguinTranform->Translate(60.0f, -10.0f, 0.0f);
	//penguinTranform->Translate(0.0f, 10.0f, 15.0f);
	penguinTranform->RotateX(90);
	penguinTranform->RotateY(-45);
	penguinTranform->Scale(1.5f, 1.5f, 1.5f);


	AddSubTree(myscene, sphinxMaterial, penguinTranform, shape);
}

/**
 * Construct the scene
 */
void ConstructScene() {

	SkyBoxShaderNode* skyboxShader = new SkyBoxShaderNode();
	if (!skyboxShader->Create("cube.vert", "cube.frag") ||
		!skyboxShader->GetLocations())
		exit(-1);
  // Shader node
  LightingShaderNode* shader = new LightingShaderNode();
  if (!shader->Create("pixel_lighting.vert", "pixel_lighting.frag") ||
      !shader->GetLocations())
    exit(-1);




  std::vector<std::string> faces{

    "arrakisday_lf.png",
	"arrakisday_rt.png", 
   "arrakisday_ft.png", 
   "arrakisday_bk.png", 
   "arrakisday_up.png", 
   "arrakisday_dn.png" 
 
  };




  SkyboxPresentation* skyboxPresentation = new SkyboxPresentation();
  SkyboxGeometry* skyboxGeometry = new SkyboxGeometry();
  skyboxGeometry->Construct(skyboxShader->GetPositionLoc());

  unsigned int cubemapTexture = skyboxPresentation->loadCubemap(faces);
  // Get the position, texture, and normal locations to use when constructing VAOs
  int position_loc = shader->GetPositionLoc();
  int normal_loc   =  shader->GetNormalLoc();
  int texture_loc  = shader->GetTextureLoc();
  int tangent_loc = shader->GetTangentLoc();
  int bitangent_loc = shader->GetBitangentLoc();

  ParticleShaderNode* particleShader = new ParticleShaderNode();
  if (!particleShader->Create("points.vert", "points.frag"))
  {
	  exit(-1);
  }
  if (!particleShader->GetLocations())
  {
	  exit(-1);
  }
 particleEmitter = new ParticleEmitter(particleShader->GetPositionLoc(), particleShader->GetColorLoc());



 std::vector<PNTVertex> vertices;
 std::vector<Vector2> uvs;
 std::vector<uint16_t> indicieList;
 ObjLoader objFile;
 bool res = objFile.loadObjFile("Anubis_Low-polyT.obj", vertices, indicieList);
 //bool res = objFile.loadObjFile("cube.obj", vertices, indicieList);

 ObjGeometry *statueShape = new ObjGeometry();
 statueShape->Construct(vertices, indicieList, position_loc, normal_loc, texture_loc);
 // Silver material (for the teapot)



 vertices.clear();
 uvs.clear();
 indicieList.clear();
 ObjLoader objFilePenguin;
 res = objFilePenguin.loadObjFile("PenguinBaseMesh.obj", vertices, indicieList);
 //bool res = objFile.loadObjFile("cube.obj", vertices, indicieList);

 ObjGeometry *penguinShape = new ObjGeometry();
 penguinShape->Construct(vertices, indicieList, position_loc, normal_loc, texture_loc);
 // Silver material (for the teapot)



 vertices.clear();
 uvs.clear();
 indicieList.clear();
 ObjLoader objFileSphynx;
 res = objFilePenguin.loadObjFile("Sphinx_Low_Poly.obj", vertices, indicieList);
 

 ObjGeometry *sphinxShape = new ObjGeometry();
 sphinxShape->Construct(vertices, indicieList, position_loc, normal_loc, texture_loc);

 PresentationNode* brickTexture = new PresentationNode(Color4(0.9f, 0.9f, 0.01f),
	 Color4(0.7f, 0.7f, 0.01f), Color4(0.5f, 0.5f, 0.5f),
	 Color4(0.0f, 0.0f, 0.0f), 50.0f);

  brickTexture->SetTexture("brickWall.jpg", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
 	 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

   brickTexture->setBumpMap("brickWallNormal.jpg", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
 	 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);


 // Transform
 TransformNode* brickTransformation = new TransformNode();
 brickTransformation->Translate(0.0f, 30.0f, 0.0f);
 //brickTransformation->RotateX(90);
 brickTransformation->Scale(50.0f, 50.0f, 50.0f);

 // Construct a textured square for the floor
 //BumpMapSquare* brickSquare = new BumpMapSquare(4, 4, position_loc,
//	 normal_loc, texture_loc, tangent_loc, bitangent_loc);
TexturedUnitSquareSurface* brickSquare = new TexturedUnitSquareSurface(4, 4, position_loc,
	 normal_loc, texture_loc);


//TexturedSphereSection* earthSphere = new TexturedSphereSection(-90.0f, 90.0f, 36,
//	-180.0f, 180.0f, 72, 1.0f, position_loc, normal_loc, texture_loc);
 PyramidGeometry* pyramid = new PyramidGeometry(1.0f, position_loc, normal_loc,
	 texture_loc);
 PresentationNode* pyramidPresentation = new PresentationNode(Color4(0.9f, 0.9f, 0.01f),
	 Color4(0.7f, 0.7f, 0.01f), Color4(0.5f, 0.5f, 0.5f),
	 Color4(0.0f, 0.0f, 0.0f), 50.0f);

 pyramidPresentation->SetTexture("brickWall.jpg", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
	 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

 pyramidPresentation->setBumpMap("brickWallNormal.jpg", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
	 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

 TransformNode* pyramidTransform = new TransformNode();
 pyramidTransform->Translate(0.0f, 200.0f, 0.0f);
 pyramidTransform->Scale(300.0f, 300.0f, 300.0f);
  // Add the camera to the scene
  // Initialize the view and set a perspective projection
  MyCamera = new CameraNode();
  MyCamera->SetPosition(Point3(0.0f, -100.0f, 20.0f));
  //MyCamera->SetPosition(Point3(0.0f, 0.0f, 0.0f));
  MyCamera->SetLookAtPt(Point3(0.0f, 0.0f, 20.0f));
  MyCamera->SetViewUp(Vector3(0.0, 0.0, 1.0));
  MyCamera->SetPerspective(50.0, 1.0, 1.0, 300.0);

  // Construct scene lighting - make lighting nodes children of the camera node
  ConstructLighting(shader);

  // Construct subdivided square - subdivided 10x in both x and y
  UnitSquareSurface* unit_square = new UnitSquareSurface(2, position_loc, normal_loc);

  // Construct a textured square for the floor
  TexturedUnitSquareSurface* textured_square = new TexturedUnitSquareSurface(2, 8, position_loc,
                 normal_loc, texture_loc);

  // Construct a unit cylinder surface
  ConicSurface* cylinder = new ConicSurface(0.5f, 0.5f, 18, 4, 
                  position_loc, normal_loc);

  // Construct a unit cone
  ConicSurface* cone = new ConicSurface(0.5f, 0.0f, 18, 4,
                  position_loc, normal_loc);

  // Construct the room as a child of the root node
  SceneNode* room = ConstructRoom(unit_square, textured_square);

  // Construct a unit box
  SceneNode* unit_box = ConstructUnitBox(unit_square);

  // Construct the table
  SceneNode* table = ConstructTable(unit_box, cylinder);

  // Wood material for table
  PresentationNode* wood = new PresentationNode(Color4(0.275f, 0.225f, 0.075f),
        Color4(0.55f, 0.45f, 0.15f), Color4(0.3f, 0.3f, 0.3f), 
        Color4(0.0f, 0.0f, 0.0f), 64.0f);

  // Position the table in the room
  TransformNode* table_transform = new TransformNode;
  table_transform->Translate(-50.0f, 50.0f, 0.0f);
  table_transform->RotateZ(30.0f);

  // Teapot
  MeshTeapot* teapot = new MeshTeapot(1, position_loc, normal_loc);

  
  /////////////////////////////////////////////////////////////////////////////////

  // Construct the scene layout
  SceneRoot = new SceneNode;
  SceneRoot->AddChild(MyCamera);
  MyCamera->AddChild(shader);

  // Construct a base node for the rest of the scene, it will be a child
  // of the last light node (so entire scene is under influence of all 
  // lights)
  SceneNode* myscene = new SceneNode;
  Spotlight->AddChild(myscene);

  ////// Add the room (walls, floor, ceiling)
  //myscene->AddChild(room);

  //// Add the table
 /* myscene->AddChild(wood);
  wood->AddChild(table_transform);
  table_transform->AddChild(table);*/



  //Skybox
  MyCamera->AddChild(skyboxShader);
  skyboxShader->AddChild(skyboxPresentation);
  skyboxPresentation->AddChild(skyboxGeometry);

  //Statue
  createEgyptianSculture(myscene, statueShape);

  createPenguin(myscene, penguinShape);

  createSphinx(myscene, sphinxShape);

  //Particle System
  MyCamera->AddChild(particleShader);
  particleShader->AddChild(particleEmitter);


  AddSubTree(myscene, pyramidPresentation, pyramidTransform, pyramid);

}

/**
 * Main 
 */
int main(int argc, char** argv) {
  // Print the keyboard commands
  std::cout << "i - Reset to initial view" << std::endl;
  std::cout << "R - Roll    5 degrees clockwise   r - Counter-clockwise" << std::endl;
  std::cout << "P - Pitch   5 degrees clockwise   p - Counter-clockwise" << std::endl;
  std::cout << "H - Heading 5 degrees clockwise   h - Counter-clockwise" << std::endl;
  std::cout << "X - Slide camera right            x - Slide camera left" << std::endl;
  std::cout << "Y - Slide camera up               y - Slide camera down" << std::endl;
  std::cout << "F - Move camera forward           f - Move camera backwards" << std::endl;
  std::cout << "V - Faster mouse movement         v - Slower mouse movement" << std::endl;
  std::cout << "ESC - Exit Program" << std::endl;

  // Initialize free GLUT
  glutInit(&argc, argv);
  glutInitContextVersion(3, 2);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  // Double buffer with depth buffer and MSAA
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowPosition(100, 100); 
  glutInitWindowSize(800, 600);
  glutCreateWindow("Desert Scene Eric Frazier and LeVar Williams");
 
  // Add GLUT callbacks
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);

  // Initialize Open 3.2 core profile
  if (gl3wInit()) {
    fprintf(stderr, "gl3wInit: failed to initialize OpenGL\n");
    return -1;
  }
  if (!gl3wIsSupported(3, 2)) {
    fprintf(stderr, "OpenGL 3.2 not supported\n");
    return -1;
  }
  printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Set the clear color to black. Any part of the window outside the
  // viewport should appear black
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Initialize DevIL
  ilInit();

  // Construct scene.
  ConstructScene();
  CheckError("After ConstructScene");

  // Enable multi-sample anti-aliasing
  glEnable(GL_MULTISAMPLE);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Enable back face polygon removal
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  // Set update rate and set initial timer callback
  glutTimerFunc((int)(1000.0f / 30), particleTimer, 0);

  glutMainLoop();
  return 0;
}