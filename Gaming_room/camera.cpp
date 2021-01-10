#include "camera.h"

/*Procesa la entrada de cualquier sistema que provenga de un teclado o Joystick,
    para ello se usa la clase enum*/
void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
    // Para que el frente no tome en cuenta el giro
    // glm::vec3 absFront = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Right)); 

    GLfloat velocity = MovementSpeed * deltaTime;
    if (direction == Camera_Movement::FORWARD)
        Position += Front * velocity;
    //Position += newFront * velocity;
    if (direction == Camera_Movement::BACKWARD)
        Position -= Front * velocity;
    //Position -= newFront * velocity;
    if (direction == Camera_Movement::LEFT)
        Position -= Right * velocity;
    if (direction == Camera_Movement::RIGHT)
        Position += Right * velocity;
}

//Procesa la entrada proveniente de un sistema de entrada del mouse y requiere el valor x e y.
void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    //Se asegura de que el pitch no esté fuera de las limites, así la pantalla no se "voltea"
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    /*Actualiza los vectores Front, Right y up usando los angulos actualizados de Euler*/
    updateCameraVectors();
}

//Recibe la entrada de la rueda de scroll de un mouse.
void Camera::ProcessMouseScroll(GLfloat yoffset)
{
    if (Zoom >= 1.0f && Zoom <= 45.0f)
        Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // Calcula el nuevo vector front
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    //También re-calula el vector Right y el vector up

    /*Se normalizan los vectores, porque sus tamaños se acercan a 0 cada vez que uno mira arriba o abajo,
    lo que resultaría en un movimiento cada vez más lento*/
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}


void Camera::IncrementaDecrementaVel(bool incre)
{
    if (incre == true)
    {
        MovementSpeed += 0.1f;
        std::cout << "La velocidad de la camara incremento a: " << MovementSpeed << std::endl;
    }
    else
    {
        MovementSpeed -= 0.1f;
        std::cout << "La velocidad de la camara decremento a: " << MovementSpeed << std::endl;
    }
    //Si superamos ciertos limites sería bueno resetearlo a la velocidad original
    if (MovementSpeed <= 0.0f || MovementSpeed >= 2.0f)
    {
        MovementSpeed = SPEED;
        std::cout << "La velocidad de la camara supera limites, reseteando a: " << MovementSpeed << std::endl;
    }
}

void Camera::imprimirPos()
{
    std::cout << "La posicion de la camara es: " << Position.x << ", " << Position.y << ", "
        << Position.z << std::endl;
}