/*****************************************************************
 * @file   Camera.hpp
 * @brief  Header for the Camera class
 * @author Wei-No Wu (weino.wu)
 * @date   November 2023
 *
 * @copyright © 2023 DigiPen (USA) Corporation.
 *****************************************************************/

#include <ext\matrix_clip_space.hpp>
#include <ext\matrix_transform.hpp>
#include <glm.hpp>
typedef struct Camera
{
private:
  bool dirty = true;
  glm::vec3 rotation = { 0,0,0 };
  float zoom = 1;
  glm::vec3 position = { 0, 0, 0 };
  glm::mat4x4 matrix;

public:
  glm::vec2 Position()
  {
    return glm::vec2(position);
  }
  glm::mat4x4 GetMatrix()
  {
    if (dirty)
    {
      
      matrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(zoom, zoom, zoom));
      matrix = glm::rotate(matrix, glm::radians(rotation.x), { 0, 0, 1 });
      matrix = glm::rotate(matrix, glm::radians(rotation.y), { 1, 0, 0 });
      matrix = glm::rotate(matrix, glm::radians(rotation.z), { 0, 1, 0 });
      matrix = glm::translate(matrix, position);
      dirty = false;
    }
    return matrix;
  }
  void moveCamera(glm::vec3 newPos)
  {
    position = newPos;
    dirty = true;
  }
  void moveCamera(glm::vec2 newPos)
  {
    position = glm::vec3(newPos, position.z);
    dirty = true;
  }
  void setZoom(float f)
  {
    zoom = f;
    dirty = true;
  };
  void rotateCamera(glm::vec3 newRot)
  {
    rotation = newRot;
    dirty = true;
  }
  float GetZoom()
  {
    return zoom;
  }
} Camera;