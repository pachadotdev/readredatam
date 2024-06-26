#ifndef REDATAMLIB_ENTITY_HPP
#define REDATAMLIB_ENTITY_HPP

#include <string>
#include <vector>
#include <memory>
#include "Variable.hpp"
#include "ICursorReader.hpp"

namespace RedatamLib {

class Entity {
public:
  // Public members
  std::string Name;
  std::vector<std::shared_ptr<Entity>> Children;
  std::string Description;
  std::string IndexFilename;
  int i1;
  std::string Alias;
  int s1;
  unsigned char b1;
  std::string RelationChild;
  std::string RelationParent;
  std::string CodesVariable;
  std::string LabelVariable;
  int Level;
  long RowsCount;
  int VariableCount;
  std::string c1;
  std::vector<std::shared_ptr<Variable>> Variables;

  // Constructor
  Entity() : reader(nullptr) {}

  // Methods
  void OpenReader() {
    if (!IndexFilename.empty()) {
      reader =
          std::make_unique<CursorReader>(ResolveDataFilename(), false, true, 0);
    } else {
      reader = std::make_unique<NullCursorReader>();
    }
  }

  long GetRowsCount() {
    long ret = 0;
    if (reader->Length() > 0) {
      long pos = 1;
      if (parentEntity && parentEntity->RowsCount > 0) {
        pos = parentEntity->RowsCount;
      }
      ret = reader->ReadInt32At(pos);
    }
    RowsCount = ret;
    return ret;
  }

  static std::vector<std::tuple<std::string, std::string>>
  Linealize(std::shared_ptr<Entity> parent,
            const std::vector<std::shared_ptr<Entity>> &entitiesNames) {
    std::vector<std::tuple<std::string, std::string>> ret;
    for (const auto &e : entitiesNames) {
      if (!parent) {
        ret.emplace_back("", e->Name);
      } else {
        ret.emplace_back(parent->Name, e->Name);
      }
      auto children = Linealize(e, e->Children);
      if (!children.empty()) {
        ret.insert(ret.end(), children.begin(), children.end());
      }
    }
    return ret;
  }

  bool DataFileExists() const {
    std::string file = ResolveDataFilename();
    std::ifstream infile(file);
    return infile.good();
  }

  std::vector<std::shared_ptr<Variable>> SelectedVariables() const {
    std::vector<std::shared_ptr<Variable>> selected;
    for (const auto &v : Variables) {
      if (v->Selected) {
        selected.push_back(v);
      }
    }
    return selected;
  }

private:
  std::unique_ptr<ICursorReader> reader;
  std::string rootPath;

  std::string ResolveDataFilename() const {
    if (!IndexFilename.empty()) {
      return rootPath + "/" + IndexFilename;
    }
    return "";
  }

  Entity *parentEntity = nullptr;
};

} // namespace RedatamLib

#endif // REDATAMLIB_ENTITY_HPP
