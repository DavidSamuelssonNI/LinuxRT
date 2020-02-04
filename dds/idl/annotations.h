/**
 * /file annotations.h
 *
 * Wrappers for accessing data from IDL annotations and registers the
 * annotation by evaluating its definition. Each annotation (@key, @topic,
 * etc.) should have class.
 *
 * To add a new annotation, implement a subclass of Annotation, implementing at
 * least definition() and name(), then add a register_one call for the class to
 * Annotations::register_all() in annotations.cpp.
 */

#ifndef OPENDDS_IDL_ANNOTATIONS_HEADER
#define OPENDDS_IDL_ANNOTATIONS_HEADER

#include <ast_expression.h>

#include <ace/Basic_Types.h>

#include <vector>
#include <string>
#include <map>

class AST_Decl;
class AST_Union;
class AST_Annotation_Decl;
class AST_Annotation_Appl;
class Annotation;

class Annotations {
public:
  Annotations();
  ~Annotations();

  void register_all();
  Annotation* operator[](const std::string& annotation);

  template<typename T>
  void register_one()
  {
    T* annotation = new T;
    map_[annotation->fullname()] = annotation;
    annotation->cache();
  }

private:
  typedef std::map<std::string, Annotation*> MapType;
  MapType map_;
};

/**
 * Wrapper Base Class for Annotations
 */
class Annotation {
public:
  Annotation();
  virtual ~Annotation();

  virtual std::string definition() const = 0;
  virtual std::string name() const = 0;
  virtual std::string fullname() const;

  AST_Annotation_Decl* declaration() const;
  AST_Annotation_Appl* find_on(AST_Decl* node) const;
  void cache();

private:
  AST_Annotation_Decl* declaration_;
};

AST_Expression::AST_ExprValue* get_annotation_member_ev(
  AST_Annotation_Appl* appl, const char* member_name);

bool get_bool_annotation_member_value(
  AST_Annotation_Appl* appl, const char* member_name);

ACE_UINT32 get_u32_annotation_member_value(
  AST_Annotation_Appl* appl, const char* member_name);

/**
 * Annotation with a Single Member Named "value"
 */
template <typename T>
class AnnotationWithValue : public Annotation {
public:
  /**
   * Default value if the node DOESN'T have the annotation. This is different
   * than the default value in the annotation definition.
   */
  T default_value() const
  {
    return T();
  }

  /**
   * Returns the value according to the annotation if it exists, else returns
   * default_value().
   */
  T node_value(AST_Decl* node) const
  {
    return value_from_appl(find_on(node));
  }

  /**
   * If node has the annotation, this sets value to the annotation value and
   * returns true.
   * If node does not have the annotation, this sets value to default_value()
   * and returns false.
   */
  bool node_value_exists(AST_Decl* node, T& value) const
  {
    AST_Annotation_Appl* appl = find_on(node);
    value = value_from_appl(appl);
    return appl;
  }

protected:
  /**
   * Get value from an annotation application. Returns default_value if appl is
   * null. Must be specialized.
   */
  T value_from_appl(AST_Annotation_Appl* appl) const;
};

template<>
bool AnnotationWithValue<bool>::value_from_appl(AST_Annotation_Appl* appl) const;

template<>
unsigned AnnotationWithValue<ACE_UINT32>::value_from_appl(
  AST_Annotation_Appl* appl) const;

template <typename T>
class AnnotationWithEnumValue : public AnnotationWithValue<T> {
protected:
  T value_from_appl(AST_Annotation_Appl* appl) const
  {
    return appl ? static_cast<T>(
      get_u32_annotation_member_value(appl, "value")) : this->default_value();
  }
};

class KeyAnnotation : public AnnotationWithValue<bool> {
public:
  std::string definition() const;
  std::string name() const;

  bool union_value(AST_Union* node) const;
};

class TopicAnnotation : public Annotation {
public:
  std::string definition() const;
  std::string name() const;
};

class NestedAnnotation : public AnnotationWithValue<bool> {
public:
  std::string definition() const;
  std::string name() const;
};

class DefaultNestedAnnotation : public AnnotationWithValue<bool> {
public:
  std::string definition() const;
  std::string name() const;
};

class IdAnnotation : public AnnotationWithValue<ACE_UINT32> {
public:
  std::string definition() const;
  std::string name() const;
};

enum AutoidKind {
  autoidkind_sequential,
  autoidkind_hash
};

class AutoidAnnotation : public AnnotationWithEnumValue<AutoidKind> {
public:
  std::string definition() const;
  std::string name() const;

  AutoidKind default_value() const;
};

class HashidAnnotation : public AnnotationWithValue<std::string> {
public:
  std::string definition() const;
  std::string name() const;

  std::string default_value() const;
};

enum ExtensibilityKind {
  extensibilitykind_final,
  extensibilitykind_appendable,
  extensibilitykind_mutable
};

class ExtensibilityAnnotation : public AnnotationWithEnumValue<ExtensibilityKind> {
public:
  std::string definition() const;
  std::string name() const;

  ExtensibilityKind default_value() const;
};

// @annotation final = @extensibility(FINAL);
class FinalAnnotation : public Annotation {
public:
  std::string definition() const;
  std::string name() const;
};

// @annotation appendable = @extensibility(APPENDABLE);
class AppendableAnnotation : public Annotation {
public:
  std::string definition() const;
  std::string name() const;
};

// @annotation mutable = @extensibility(MUTABLE);
class MutableAnnotation : public Annotation {
public:
  std::string definition() const;
  std::string name() const;
};

#endif
