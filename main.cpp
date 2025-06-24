#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Window dimensions
#define WIDTH 800
#define HEIGHT 600

// Bin & Lid constants
#define LID_THICKNESS 0.15f
#define RIM_HEIGHT    0.2f

// Camera (mouse interaction)
float cameraYaw = 0.0f;    // Horizontal orbit angle (degrees)
float cameraPitch = 20.0f; // Vertical orbit angle (degrees)
float cameraDistance = 20.0f;
int lastMouseX, lastMouseY;
int mouseButton = -1;

// Global variables
GLfloat noEmission[4] = {0.0f, 0.0f, 0.0f, 1.0f};

// Define bin colors
GLfloat recyclableBinColor[3] = {0.0f, 0.7f, 0.3f}; // Brighter green
GLfloat organicBinColor[3] = {1.0f, 0.6f, 0.0f};    // Brighter orange
GLfloat hazardousBinColor[3] = {0.9f, 0.1f, 0.1f};  // Brighter red

// Function prototypes
void init();
void display();
void reshape(int width, int height);
void drawGarbageBin();
void drawUnifiedBinContainer(float width, float height, float depth, const GLfloat color[3]);
void drawBinDivider(float x, float y, float z, float height, float depth, const GLfloat color[3]);
void drawLid(float width, float depth, const GLfloat color[3]);
void drawCylinder(float radius, float height, int segments);
void drawRecycleSymbol(float x, float y, float z, float size);
void drawLeafSymbol(float x, float y, float z, float size);
void drawHazardSymbol(float x, float y, float z, float size);
void drawCompartmentLabels();
void drawGround();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);

// Initialize OpenGL
void init() {
    glClearColor(0.8f, 0.8f, 0.9f, 1.0f); // Light blue-gray background
    glEnable(GL_DEPTH_TEST);

    // Enhanced lighting setup
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Increase ambient light to make objects more visible
    GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f}; // Brighter ambient light
    GLfloat diffuseLight[] = {0.7f, 0.7f, 0.7f, 1.0f}; // Brighter diffuse light
    GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Full intensity specular
    GLfloat position[] = {-5.0f, 15.0f, 10.0f, 1.0f}; // Better position

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    // Add a second light source from a different angle
    glEnable(GL_LIGHT1);
    GLfloat ambientLight2[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuseLight2[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat position2[] = {10.0f, 10.0f, -5.0f, 1.0f}; // Light from opposite direction

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight2);
    glLightfv(GL_LIGHT1, GL_POSITION, position2);

    // Enable color tracking
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Enable normalization of normal vectors
    glEnable(GL_NORMALIZE);

    // Enable line smoothing for better looking lines
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

// Main display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Spherical-to-Cartesian camera orbit calculation (look at bin center)
    float camX = cameraDistance * cosf(cameraPitch * M_PI / 180.0f) * sinf(cameraYaw * M_PI / 180.0f);
    float camY = cameraDistance * sinf(cameraPitch * M_PI / 180.0f);
    float camZ = cameraDistance * cosf(cameraPitch * M_PI / 180.0f) * cosf(cameraYaw * M_PI / 180.0f);

    gluLookAt(
        camX, camY + 2.0f, camZ,  // Camera position (Y+2 centers the bin in view)
        0.0f, 2.0f, 0.0f,         // Look at
        0.0f, 1.0f, 0.0f          // Up
    );

    // Draw ground
    drawGround();

    // Draw bin system
    drawGarbageBin();

    glutSwapBuffers();
}

// Handle window reshape
void reshape(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
}

// Mouse and motion interaction
void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        mouseButton = button;
        lastMouseX = x;
        lastMouseY = y;
    } else {
        mouseButton = -1;
    }
}

void motion(int x, int y) {
    int dx = x - lastMouseX;
    int dy = y - lastMouseY;
    if (mouseButton == GLUT_LEFT_BUTTON) { // Orbit
        cameraYaw += dx * 0.5f;
        cameraPitch += dy * 0.5f;
        if (cameraPitch > 89.0f) cameraPitch = 89.0f;
        if (cameraPitch < -89.0f) cameraPitch = -89.0f;
    }
    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

// Keyboard handler for zoom
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+':
        case '=': // Allow both + and = for zoom in
            cameraDistance -= 1.0f;
            if (cameraDistance < 5.0f) cameraDistance = 5.0f;
            glutPostRedisplay();
            break;
        case '-':
        case '_': // Allow both - and _ for zoom out
            cameraDistance += 1.0f;
            if (cameraDistance > 50.0f) cameraDistance = 50.0f;
            glutPostRedisplay();
            break;
        case 27: // ESC
            exit(0);
            break;
    }
}

// Draw the ground
void drawGround() {
    GLfloat groundColor[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat groundSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, groundSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);

    glPushMatrix();
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-20.0f, 0.0f, -20.0f);
    glVertex3f(-20.0f, 0.0f, 20.0f);
    glVertex3f(20.0f, 0.0f, 20.0f);
    glVertex3f(20.0f, 0.0f, -20.0f);
    glEnd();
    glPopMatrix();
}

// Draw a cylinder
void drawCylinder(float radius, float height, int segments) {
    GLUquadricObj* quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);

    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadric, radius, radius, height, segments, 1);
    gluDisk(quadric, 0.0f, radius, segments, 1);
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quadric, 0.0f, radius, segments, 1);
    glPopMatrix();

    gluDeleteQuadric(quadric);
}

// Draw complete garbage bin system
void drawGarbageBin() {
    // Base platform colors
    GLfloat baseColor[3] = {0.8f, 0.8f, 0.85f};

    // Draw the base
    GLfloat baseAmbient[4] = {baseColor[0] * 0.6f, baseColor[1] * 0.6f, baseColor[2] * 0.6f, 1.0f};
    GLfloat baseDiffuse[4] = {baseColor[0], baseColor[1], baseColor[2], 1.0f};
    GLfloat baseSpecular[4] = {0.4f, 0.4f, 0.4f, 1.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, baseAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, baseDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, baseSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, 30.0f);

    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 0.0f);

    // Bin proportions
    float totalWidth = 12.0f;
    float binDepth = 6.0f;
    float binHeight = 5.0f;

    // Draw main bin container (shared body)
    GLfloat binColor[3] = {0.7f, 0.7f, 0.7f}; // Neutral color for bin body

    glPushMatrix();
    glTranslatef(0.0f, 2.1f, 0.0f);

    // Draw the shared container body
    drawUnifiedBinContainer(totalWidth, binHeight, binDepth, binColor);

    // Dividers - correct positions/heights so flush with bin top (not rim)
    GLfloat dividerColor[3] = {0.5f, 0.5f, 0.5f};
    float dividerHeight = binHeight - RIM_HEIGHT;
    float dividerY = -binHeight/2 + dividerHeight/2;
    drawBinDivider(-2.0f, dividerY, 0.0f, dividerHeight, binDepth * 0.9f, dividerColor);
    drawBinDivider(2.0f, dividerY, 0.0f, dividerHeight, binDepth * 0.9f, dividerColor);

    // Draw the three colored lids - flush with rim + bin, not floating
    float compartmentWidth = 3.9f;
    float lidY = binHeight/2 - RIM_HEIGHT - LID_THICKNESS/2;

    glPushMatrix();
    glTranslatef(-4.0f, lidY, 0.0f);
    drawLid(compartmentWidth, binDepth * 1.0f, recyclableBinColor);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, lidY, 0.0f);
    drawLid(compartmentWidth, binDepth * 1.0f, organicBinColor);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.0f, lidY, 0.0f);
    drawLid(compartmentWidth, binDepth * 1.0f, hazardousBinColor);
    glPopMatrix();

    glPopMatrix();

    // Adjust compartment labels for new dimensions
    drawCompartmentLabels();

    glPopMatrix();
}


// Draw unified bin container
void drawUnifiedBinContainer(float width, float height, float depth, const GLfloat color[3]) {
    GLfloat brighterColor[3] = {
        color[0] * 1.5f > 1.0f ? 1.0f : color[0] * 1.5f,
        color[1] * 1.5f > 1.0f ? 1.0f : color[1] * 1.5f,
        color[2] * 1.5f > 1.0f ? 1.0f : color[2] * 1.5f
    };

    GLfloat ambient[4] = {brighterColor[0] * 0.6f, brighterColor[1] * 0.6f, brighterColor[2] * 0.6f, 1.0f};
    GLfloat diffuse[4] = {brighterColor[0], brighterColor[1], brighterColor[2], 1.0f};
    GLfloat specular[4] = {0.4f, 0.4f, 0.4f, 1.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

    float w = width / 2.0f;
    float h = height / 2.0f;
    float d = depth / 2.0f;
    float cornerRadius = 0.5f;
    float widthScale = 1.05f;
    float depthScale = 1.05f;

    glPushMatrix();

    // Draw the main bin faces
    glBegin(GL_QUADS);

    // Front face - tapered
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w, -h + cornerRadius, d);
    glVertex3f(w, -h + cornerRadius, d);
    glVertex3f(w * widthScale, h - cornerRadius, d * depthScale);
    glVertex3f(-w * widthScale, h - cornerRadius, d * depthScale);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-w, -h + cornerRadius, -d);
    glVertex3f(-w * widthScale, h - cornerRadius, -d * depthScale);
    glVertex3f(w * widthScale, h - cornerRadius, -d * depthScale);
    glVertex3f(w, -h + cornerRadius, -d);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w, -h + cornerRadius, -d);
    glVertex3f(-w, -h + cornerRadius, d);
    glVertex3f(-w * widthScale, h - cornerRadius, d * depthScale);
    glVertex3f(-w * widthScale, h - cornerRadius, -d * depthScale);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w, -h + cornerRadius, -d);
    glVertex3f(w * widthScale, h - cornerRadius, -d * depthScale);
    glVertex3f(w * widthScale, h - cornerRadius, d * depthScale);
    glVertex3f(w, -h + cornerRadius, d);
    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-w, -h + cornerRadius, -d);
    glVertex3f(w, -h + cornerRadius, -d);
    glVertex3f(w, -h + cornerRadius, d);
    glVertex3f(-w, -h + cornerRadius, d);
    glEnd();

    // ADDED: Rounded corners using cylinders at bottom
    GLfloat cornerColor[4] = {brighterColor[0] * 0.9f, brighterColor[1] * 0.9f, brighterColor[2] * 0.9f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cornerColor);

    GLUquadricObj* cornerQuad = gluNewQuadric();
    gluQuadricNormals(cornerQuad, GLU_SMOOTH);

    // Bottom corners (4 corners)
    float bottomY = -h + cornerRadius;

    // Bottom front left
    glPushMatrix();
    glTranslatef(-w, bottomY, d);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    gluPartialDisk(cornerQuad, 0.0f, cornerRadius, 12, 1, 0, 90);
    glPopMatrix();

    // Bottom front right
    glPushMatrix();
    glTranslatef(w, bottomY, d);
    glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
    gluPartialDisk(cornerQuad, 0.0f, cornerRadius, 12, 1, 0, 90);
    glPopMatrix();

    // Bottom back right
    glPushMatrix();
    glTranslatef(w, bottomY, -d);
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
    gluPartialDisk(cornerQuad, 0.0f, cornerRadius, 12, 1, 0, 90);
    glPopMatrix();

    // Bottom back left
    glPushMatrix();
    glTranslatef(-w, bottomY, -d);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluPartialDisk(cornerQuad, 0.0f, cornerRadius, 12, 1, 0, 90);
    glPopMatrix();

    // Horizontal grooves
    GLfloat grooveColor[4] = {brighterColor[0] * 0.7f, brighterColor[1] * 0.7f, brighterColor[2] * 0.7f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, grooveColor);

    for (int i = 1; i < 5; i++) {
        float y = -h + height * 0.2f * i;
        glBegin(GL_LINES);
        glLineWidth(2.0f);

        glVertex3f(-w * 0.95f, y, d * 1.01f);
        glVertex3f(w * 0.95f, y, d * 1.01f);

        glVertex3f(-w * 1.01f, y, -d * 0.9f);
        glVertex3f(-w * 1.01f, y, d * 0.9f);

        glVertex3f(w * 1.01f, y, -d * 0.9f);
        glVertex3f(w * 1.01f, y, d * 0.9f);
        glEnd();
    }

    // Rim (top edge)
    GLfloat rimColor[4] = {brighterColor[0] * 0.8f, brighterColor[1] * 0.8f, brighterColor[2] * 0.8f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, rimColor);

    float rimHeight = RIM_HEIGHT;
    glBegin(GL_QUAD_STRIP);
    glVertex3f(-w * widthScale * 1.02f, h, -d * depthScale * 1.02f);
    glVertex3f(-w * widthScale * 1.02f, h - rimHeight, -d * depthScale * 1.02f);

    glVertex3f(-w * widthScale * 1.02f, h, d * depthScale * 1.02f);
    glVertex3f(-w * widthScale * 1.02f, h - rimHeight, d * depthScale * 1.02f);

    glVertex3f(w * widthScale * 1.02f, h, d * depthScale * 1.02f);
    glVertex3f(w * widthScale * 1.02f, h - rimHeight, d * depthScale * 1.02f);

    glVertex3f(w * widthScale * 1.02f, h, -d * depthScale * 1.02f);
    glVertex3f(w * widthScale * 1.02f, h - rimHeight, -d * depthScale * 1.02f);

    glVertex3f(-w * widthScale * 1.02f, h, -d * depthScale * 1.02f);
    glVertex3f(-w * widthScale * 1.02f, h - rimHeight, -d * depthScale * 1.02f);
    glEnd();

    glPopMatrix();

    // Bin feet (corners)
    GLfloat footColor[4] = {0.3f, 0.3f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, footColor);
    glMaterialfv(GL_FRONT, GL_AMBIENT, footColor);
    float footSize = 0.4f;
    float footHeight = 0.2f;
    float xOffsets[4] = {-w * 0.85f, w * 0.85f, w * 0.85f, -w * 0.85f};
    float zOffsets[4] = {-d * 0.85f, -d * 0.85f, d * 0.85f, d * 0.85f};
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(xOffsets[i], -h, zOffsets[i]);
        drawCylinder(footSize, footHeight, 8);
        glPopMatrix();
    }
    gluDeleteQuadric(cornerQuad);
}


// Draw bin divider
void drawBinDivider(float x, float y, float z, float height, float depth, const GLfloat color[3]) {
    GLfloat ambient[4] = {color[0] * 0.6f, color[1] * 0.6f, color[2] * 0.6f, 1.0f};
    GLfloat diffuse[4] = {color[0], color[1], color[2], 1.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.1f, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Draw a modern, sleek lid design
void drawLid(float width, float depth, const GLfloat color[3]) {
    float w = width / 2.0f;
    float d = depth / 2.0f;
    float thickness = LID_THICKNESS;

    // Calculate slightly lighter color for lid
    GLfloat lidColor[3] = {
        color[0] * 1.6f > 1.0f ? 1.0f : color[0] * 1.6f,
        color[1] * 1.6f > 1.0f ? 1.0f : color[1] * 1.6f,
        color[2] * 1.6f > 1.0f ? 1.0f : color[2] * 1.6f
    };

    // Set material properties
    GLfloat ambient[4] = {lidColor[0] * 0.6f, lidColor[1] * 0.6f, lidColor[2] * 0.6f, 1.0f};
    GLfloat diffuse[4] = {lidColor[0], lidColor[1], lidColor[2], 1.0f};
    GLfloat specular[4] = {0.8f, 0.8f, 0.8f, 1.0f}; // More reflective

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 60.0f); // Higher shininess

    glPushMatrix();

    // Main lid surface
    glBegin(GL_QUADS);
    int segments = 10;
    float segmentWidth = 2.0f * w / segments;

    for (int i = 0; i < segments; i++) {
        float x1 = -w + i * segmentWidth;
        float x2 = x1 + segmentWidth;
        float y1 = thickness/2.0f + sin((float)i/segments * M_PI) * 0.1f;
        float y2 = thickness/2.0f + sin((float)(i+1)/segments * M_PI) * 0.1f;

        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x1, y1, -d);
        glVertex3f(x1, y1, d);
        glVertex3f(x2, y2, d);
        glVertex3f(x2, y2, -d);
    }

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-w, -thickness/2.0f, -d);
    glVertex3f(w, -thickness/2.0f, -d);
    glVertex3f(w, -thickness/2.0f, d);
    glVertex3f(-w, -thickness/2.0f, d);

    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w, -thickness/2.0f, d);
    glVertex3f(w, -thickness/2.0f, d);
    glVertex3f(w, thickness/2.0f, d);
    glVertex3f(-w, thickness/2.0f, d);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-w, -thickness/2.0f, -d);
    glVertex3f(-w, thickness/2.0f, -d);
    glVertex3f(w, thickness/2.0f, -d);
    glVertex3f(w, -thickness/2.0f, -d);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w, -thickness/2.0f, -d);
    glVertex3f(-w, -thickness/2.0f, d);
    glVertex3f(-w, thickness/2.0f, d);
    glVertex3f(-w, thickness/2.0f, -d);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w, -thickness/2.0f, -d);
    glVertex3f(w, thickness/2.0f, -d);
    glVertex3f(w, thickness/2.0f, d);
    glVertex3f(w, -thickness/2.0f, d);
    glEnd();

    // Add raised edge for grip
    float edgeThickness = 0.2f;
    GLfloat edgeColor[4] = {lidColor[0] * 0.9f, lidColor[1] * 0.9f, lidColor[2] * 0.9f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, edgeColor);

    glBegin(GL_QUADS);
    // Top of edge
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-w * 0.8f, thickness/2.0f, d);
    glVertex3f(-w * 0.8f, thickness/2.0f, d + edgeThickness);
    glVertex3f(w * 0.8f, thickness/2.0f, d + edgeThickness);
    glVertex3f(w * 0.8f, thickness/2.0f, d);

    // Front of edge
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w * 0.8f, -thickness/2.0f, d + edgeThickness);
    glVertex3f(w * 0.8f, -thickness/2.0f, d + edgeThickness);
    glVertex3f(w * 0.8f, thickness/2.0f, d + edgeThickness);
    glVertex3f(-w * 0.8f, thickness/2.0f, d + edgeThickness);

    // Left of edge
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w * 0.8f, -thickness/2.0f, d);
    glVertex3f(-w * 0.8f, -thickness/2.0f, d + edgeThickness);
    glVertex3f(-w * 0.8f, thickness/2.0f, d + edgeThickness);
    glVertex3f(-w * 0.8f, thickness/2.0f, d);

    // Right of edge
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w * 0.8f, -thickness/2.0f, d);
    glVertex3f(w * 0.8f, thickness/2.0f, d);
    glVertex3f(w * 0.8f, thickness/2.0f, d + edgeThickness);
    glVertex3f(w * 0.8f, -thickness/2.0f, d + edgeThickness);
    glEnd();

    // Add edge highlighting
    GLfloat highlightColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, highlightColor);
    glMaterialfv(GL_FRONT, GL_AMBIENT, highlightColor);

    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float x = -w + i * segmentWidth;
        float y = thickness/2.0f + sin((float)i/segments * M_PI) * 0.1f;
        glVertex3f(x, y, d);
    }
    glVertex3f(w, thickness/2.0f + sin(1.0f * M_PI) * 0.1f, d);
    glVertex3f(w, thickness/2.0f + sin(1.0f * M_PI) * 0.1f, -d);

    for (int i = segments; i > 0; i--) {
        float x = -w + i * segmentWidth;
        float y = thickness/2.0f + sin((float)i/segments * M_PI) * 0.1f;
        glVertex3f(x, y, -d);
    }
    glVertex3f(-w, thickness/2.0f + sin(0.0f) * 0.1f, -d);
    glEnd();

    glPopMatrix();
}

// Draw a recycle symbol
void drawRecycleSymbol(float x, float y, float z, float size) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Use a solid white color with high emission for visibility
    GLfloat symbolColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat emissionColor[4] = {0.3f, 0.3f, 0.3f, 1.0f};

    glMaterialfv(GL_FRONT, GL_DIFFUSE, symbolColor);
    glMaterialfv(GL_FRONT, GL_AMBIENT, symbolColor);
    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);

    glLineWidth(2.0f);

    float arrowSize = size * 0.3f;
    float offset = size * 0.2f;

    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glRotatef(i * 120.0f, 0.0f, 0.0f, 1.0f);
        glTranslatef(0.0f, offset, 0.0f);

        glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, arrowSize, 0.0f);
        glVertex3f(-arrowSize * 0.5f, 0.0f, 0.0f);
        glVertex3f(arrowSize * 0.5f, 0.0f, 0.0f);
        glEnd();

        glBegin(GL_QUADS);
        glVertex3f(-arrowSize * 0.2f, 0.0f, 0.0f);
        glVertex3f(arrowSize * 0.2f, 0.0f, 0.0f);
        glVertex3f(arrowSize * 0.2f, -arrowSize * 1.5f, 0.0f);
        glVertex3f(-arrowSize * 0.2f, -arrowSize * 1.5f, 0.0f);
        glEnd();

        glPopMatrix();
    }

    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);

    glPopMatrix();
}

// Draw a leaf symbol for organic waste
void drawLeafSymbol(float x, float y, float z, float size) {
    glPushMatrix();
    glTranslatef(x, y, z);

    GLfloat symbolColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat emissionColor[4] = {0.3f, 0.3f, 0.3f, 1.0f};

    glMaterialfv(GL_FRONT, GL_DIFFUSE, symbolColor);
    glMaterialfv(GL_FRONT, GL_AMBIENT, symbolColor);
    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, size * 0.5f, 0.0f);
    int segments = 12;
    for (int i = 0; i <= segments; i++) {
        float angle = M_PI * i / segments;
        float leafWidth = sin(angle) * size * 0.4f;
        float leafLength = -cos(angle) * size * 0.8f;
        glVertex3f(leafWidth, leafLength, 0.0f);
    }
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-size * 0.05f, -size * 0.3f, 0.0f);
    glVertex3f(size * 0.05f, -size * 0.3f, 0.0f);
    glVertex3f(size * 0.05f, -size * 0.8f, 0.0f);
    glVertex3f(-size * 0.05f, -size * 0.8f, 0.0f);
    glEnd();

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, size * 0.5f, 0.0f);
    glVertex3f(0.0f, -size * 0.3f, 0.0f);
    for (int i = 1; i <= 4; i++) {
        float veinPos = -size * 0.3f + i * (size * 0.8f / 5);
        float veinWidth = sin(i * M_PI / 10) * size * 0.35f;
        glVertex3f(0.0f, veinPos, 0.0f);
        glVertex3f(-veinWidth, veinPos - size * 0.1f, 0.0f);
        glVertex3f(0.0f, veinPos, 0.0f);
        glVertex3f(veinWidth, veinPos - size * 0.1f, 0.0f);
    }
    glEnd();

    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);

    glPopMatrix();
}

// Draw a hazard symbol
void drawHazardSymbol(float x, float y, float z, float size) {
    glPushMatrix();
    glTranslatef(x, y, z);

    GLfloat symbolColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat emissionColor[4] = {0.3f, 0.3f, 0.3f, 1.0f};

    glMaterialfv(GL_FRONT, GL_DIFFUSE, symbolColor);
    glMaterialfv(GL_FRONT, GL_AMBIENT, symbolColor);
    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);

    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, size * 0.6f, 0.0f);
    glVertex3f(-size * 0.5f, -size * 0.4f, 0.0f);
    glVertex3f(size * 0.5f, -size * 0.4f, 0.0f);
    glEnd();

    GLfloat borderColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, borderColor);
    glMaterialfv(GL_FRONT, GL_AMBIENT, borderColor);
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);

    float borderWidth = size * 0.05f;
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, size * 0.6f - borderWidth, 0.0f);
    glVertex3f(-size * 0.5f + borderWidth, -size * 0.4f + borderWidth, 0.0f);
    glVertex3f(size * 0.5f - borderWidth, -size * 0.4f + borderWidth, 0.0f);
    glEnd();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, symbolColor);
    glMaterialfv(GL_FRONT, GL_AMBIENT, symbolColor);
    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);

    glBegin(GL_QUADS);
    glVertex3f(-size * 0.05f, size * 0.2f, 0.0f);
    glVertex3f(size * 0.05f, size * 0.2f, 0.0f);
    glVertex3f(size * 0.05f, -size * 0.2f, 0.0f);
    glVertex3f(-size * 0.05f, -size * 0.2f, 0.0f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-size * 0.05f, -size * 0.25f, 0.0f);
    glVertex3f(size * 0.05f, -size * 0.25f, 0.0f);
    glVertex3f(size * 0.05f, -size * 0.35f, 0.0f);
    glVertex3f(-size * 0.05f, -size * 0.35f, 0.0f);
    glEnd();

    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);

    glPopMatrix();
}

// Draw compartment labels (symbols) on the front face of the bin
void drawCompartmentLabels() {
    float labelY = 2.1f;   // Centered vertically on the bin
    float labelZ = 3.2f;   // Slightly in front of the bin's front face
    float labelSize = 1.0f;

    drawRecycleSymbol(-4.0f, labelY, labelZ, labelSize);
    drawLeafSymbol(0.0f, labelY, labelZ, labelSize);
    drawHazardSymbol(4.0f, labelY, labelZ, labelSize);
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Smart Waste Management - Unified Sorting Bin (Mouse + Keyboard Zoom)");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    printf("Controls:\n");
    printf("Left mouse drag: Orbit camera\n");
    printf("+: Zoom in\n");
    printf("-: Zoom out\n");
    printf("ESC: Exit\n");

    glutMainLoop();
    return 0;
}



