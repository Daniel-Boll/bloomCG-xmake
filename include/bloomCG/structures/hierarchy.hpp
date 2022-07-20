#pragma once

#include <bloomCG/core/camera.hpp>
#include <bloomCG/core/common.hpp>
#include <bloomCG/models/cube.hpp>
#include <bloomCG/models/light.hpp>
#include <bloomCG/models/sphere.hpp>

namespace bloom {

  // Each key represents the type of the constructor of the class
  enum class ObjectType { CUBE, SPHERE, LIGHT, CAMERA };
  struct Objects {
    ObjectType type;
    std::string name;
    int32_t index;

    union Object {
      bloom::Sphere* sphere;
      bloom::Cube* cube;
      bloom::Light* light;
      bloom::Camera* camera;
    } object;

    bloom::Entity* get() {
      // Check for the current type of the struct, then return the object
      switch (type) {
        case ObjectType::CUBE:
          return object.cube;
        case ObjectType::SPHERE:
          return object.sphere;
        case ObjectType::LIGHT:
          return object.light;
        case ObjectType::CAMERA:
          return object.camera;
      }
    }
  };

  inline std::ostream& operator<<(std::ostream& os, const Objects& object) {
    os << "Object: " << object.name << " (" << object.index << ")";
    return os;
  }

  // Hierarchy
  std::vector<Objects> hierarchyObjects;

  // Get reference of the object by type
  template <ObjectType T> Objects& getObjectByTypeRef(int32_t index) {
    // Retrieve the index of the object in the hierarchy that matches the type and index
    std::for_each(hierarchyObjects.begin(), hierarchyObjects.end(), [&index](Objects& object) {
      if (object.type == T && object.index == index) {
        index = object.index;
      }
    });

    // Return the object
    return hierarchyObjects[index];
  }

  // Get all objects of the hierarchy by type, if index is informed, it will get the object at the
  // index of the vector
  template <ObjectType T> std::vector<Objects> getObjectByType() {
    std::vector<Objects> objects;
    std::copy_if(hierarchyObjects.begin(), hierarchyObjects.end(), std::back_inserter(objects),
                 [](const Objects& obj) { return obj.type == T; });
    return objects;
  }

  template <ObjectType T> Objects getObjectByType(int32_t index) {
    std::vector<Objects> objects = getObjectByType<T>();

    if (objects.size() == 0) return Objects{};

    return objects[index];
  }

}  // namespace bloom
