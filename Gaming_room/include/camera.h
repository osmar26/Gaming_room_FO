#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

/*Con esta enumeracion definimos varias opciones para el movimiento de la camara.
Es una abstracción, de esta forma la entrada puede ser por teclado o por Joystick por ejemplo*/
enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Las propiedades de la camara por defecto
const GLfloat YAW         = -90.0f;
const GLfloat PITCH       =  0.0f;
const GLfloat SPEED       =  0.5f;
const GLfloat SENSITIVITY =  0.7f;
const GLfloat ZOOM        =  45.0f;


// La camara calcula los angulos de euler, matrices y vectores para que se pueda usar con OpenGL

class Camera
{
public:
    // Constructor con vectores, utilizando lista de inicializacion de atributos
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        GLfloat yaw = YAW, GLfloat pitch = PITCH) : Position{ position }, WorldUp{ up }, Yaw{ yaw }, Pitch{ pitch },
        Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        updateCameraVectors();
    }

    // Constructor con valores escalares, se lo delegamos al otro constructor
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
        : Camera(glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ), yaw, pitch) {    }

    //Retorna la matriz de vista calculada usando los angulos de Euler y la matriz "Look At"
    //Implicitamente deberían de ser inline pero se especifica para que sea más claro
    inline glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    /*Procesa la entrada de cualquier sistema que provenga de un teclado o Joystick,
    para ello se usa la clase enum*/
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);

    //Procesa la entrada proveniente de un sistema de entrada del mouse y requiere el valor x e y.
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);

    //Recibe la entrada de la rueda de scroll de un mouse.
    void ProcessMouseScroll(GLfloat yoffset);

    inline glm::vec3 getPosition() const
    {
        return Position;
    }

    inline GLfloat getZoom() const
    {
        return Zoom;
    }

private:

    // Atributos de la camara
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Los angulos de euler
    GLfloat Yaw;
    GLfloat Pitch;
    // Opciones de la camara
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    //Calcula el vector fron de la camara con sus angulos actializados de Euler
    void updateCameraVectors();
};
#endif