#include <gl/glut.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Variables de visión estereoscópica
float eyeSeparation = 0.1f;
float cameraZ = 5.0f;

// Variables para la rotación de la escena
float angleX = 0.0f, angleY = 0.0f;

GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (data == nullptr) {
        std::cerr << "Error al cargar la imagen: " << filename << std::endl;
        exit(1);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}
GLuint backgroundTexture;

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float lightPos[] = { 0.0f, 0.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    backgroundTexture = loadTexture("C:\\Users\\KiwiG\\Downloads\\parque.jpg");
}

void renderBackground(float aspectRatio) {
    glPushMatrix();

    glDisable(GL_LIGHTING);  // Desactivar iluminación para el fondo
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glEnable(GL_TEXTURE_2D);

    // Set the texture environment mode
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f); // Coloca el plano de fondo detrás del cubo

    float height = 5.0f;
    float width = height * aspectRatio;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-width, -height, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, -height, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, height, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, height, 0.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);  // Reactivar iluminación
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}

void renderScene() {
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(30.0f, 1.0, 0.0, 0.0); // Inclinación adicional
    glRotatef(45.0f, 0.0, 1.0, 0.0); // Inclinación adicional
    glColor3f(0.8f, 0.8f, 0.8f);
    glutSolidCube(1.0);

    glTranslatef(-1.5f, 1.0f, 0.0f);
    glRotatef(60.0f, 1.0, 0.0, 0.0); // Inclinación adicional
    glRotatef(15.0f, 0.0, 1.0, 0.0); // Inclinación adicional
    glColor3f(0.2f, 0.6f, 0.8f);
    glutSolidCube(0.5);

    glTranslatef(2.5f, -1.0f, -0.5f);
    glRotatef(45.0f, 1.0, 0.0, 0.0); // Inclinación adicional
    glRotatef(30.0f, 0.0, 1.0, 0.0); // Inclinación adicional
    glColor3f(0.9f, 0.3f, 0.2f);
    glutSolidCube(0.5);
    glPopMatrix();
}

void renderAnaglyphScene(float aspectRatio) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Renderizar el fondo sin aplicar el anaglifo (con todos los colores)
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    renderBackground(aspectRatio);


    glClear(GL_DEPTH_BUFFER_BIT);  // Limpiar solo el buffer de profundidad

    // Ojo izquierdo (Rojo)
    glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
    glLoadIdentity();
    gluLookAt(-eyeSeparation, 0.0, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    renderScene();
    glPopMatrix();

    // Ojo derecho (Cian)
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
    glLoadIdentity();
    gluLookAt(eyeSeparation, 0.0, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    renderScene();
    glPopMatrix();

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    float aspectRatio = (float)w / (float)h;

    renderAnaglyphScene(aspectRatio);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': angleX += 5.0f; break;
    case 's': angleX -= 5.0f; break;
    case 'a': angleY -= 5.0f; break;
    case 'd': angleY += 5.0f; break;
    case 27: exit(0);
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Visión Estereoscópica con Anaglifo");

    initOpenGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
