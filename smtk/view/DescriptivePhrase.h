//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef smtk_view_DescriptivePhrase_h
#define smtk_view_DescriptivePhrase_h

#include "smtk/PublicPointerDefs.h"
#include "smtk/SharedFromThis.h"

#include "smtk/view/PhraseContent.h"

#include "smtk/common/UUID.h"

#include "smtk/resource/PropertyType.h"

#include <string>
#include <vector>

namespace smtk
{
namespace view
{

class Badge;
class DescriptivePhrase;
class SubphraseGenerator;
typedef smtk::shared_ptr<SubphraseGenerator> SubphraseGeneratorPtr;
typedef std::vector<DescriptivePhrasePtr> DescriptivePhrases;

/// Possible types of relationships that the iterator will report
enum class DescriptivePhraseType
{
  RESOURCE_SUMMARY,       //!< Summarize a resource by name, filename, and perhaps timestamp.
  RESOURCE_LIST,          //!< This phrase summarizes multiple resources.
  COMPONENT_SUMMARY,      //!< Summarize a component by displaying its name.
  COMPONENT_LIST,         //!< This phrase summarizes multiple components.
  PROPERTY_LIST,          //!< Summarize a list of properties (usually of a single component).
  FLOAT_PROPERTY_VALUE,   //!< Show floating-point values for a named property.
  STRING_PROPERTY_VALUE,  //!< Show string values for a named property.
  INTEGER_PROPERTY_VALUE, //!< Show integer values for a named property.
  LIST,                   //!< A list of descriptive phrases with no constraint on their type.
  INVALID_DESCRIPTION     //!< This is used to indicate an invalid or empty descriptive phrase.
};

/**\brief A base class for phrases describing an SMTK model.
  *
  * Instances of subclasses serve as the basis for user interfaces
  * to display information about the model in a hierarchical fashion.
  * Each phrase may have zero-to-many subphrases of any type and
  * zero-or-one parent phrases.
  *
  * The title(), subtitle(), and phraseType() report information
  * for the user interface to present. In the Qt layer, these are
  * to be used for list item text (title and subtitle) and icon
  * (phraseType).
  *
  * Subphrases are built on demand so that portions of the UI
  * may be presented. Subphrases are not generated by instances of
  * this class, but by instances of a delegate SubphraseGenerator
  * class.
  *
  * Any DescriptivePhrase may own a SubphraseGenerator, indicating
  * that its children should be computed with that generator.
  * More than one phrase may point to the same instance of SubphraseGenerator.
  * However, the more convenient alternative is for only the top-level
  * phrase of the tree to point to a subphrase generator;
  * if a phrase does not have a generator but does have a parent, then
  * it will use its parent's generator.
  */
class SMTKCORE_EXPORT DescriptivePhrase : smtkEnableSharedPtr(DescriptivePhrase)
{
public:
  /** \brief The signature of a function used to visit all or part of a phrase hierarchy.
    *
    * The second argument is a vector of integers specifying the indices of the phrase in
    * its parent phrases (or empty if it has no parent). The back of the vector is the
    * index in its immediate parent while the front holds the index of the top-level phrase
    * to choose in order to descent the hierarchy to the current phrase, as if argFindChild()
    * was called on each phrase during descent.
    *
    * The return value indicates how the hiearchy traversal should continue:
    * return 0 to continue iterating, 1 to skip children of this phrase, or 2 to terminate immediately.
    */
  using Visitor = std::function<int(DescriptivePhrasePtr, std::vector<int>&)>;
  using Badges = std::vector<Badge*>;

  smtkTypeMacroBase(DescriptivePhrase);
  smtkCreateMacro(DescriptivePhrase);
  virtual ~DescriptivePhrase() {}

  /// Populate a phrase with its type and parent in the hierarchy of phrases.
  Ptr setup(DescriptivePhraseType phraseType, Ptr parent = Ptr());
  /// Set the subphrase generator used to populate the children of this phrase.
  Ptr setDelegate(SubphraseGeneratorPtr delegate);
  /// Set the content (state) of the phrase
  bool setContent(PhraseContentPtr content);
  /// Return the content (state) of the phrase.
  PhraseContentPtr content() const;
  /// Return an ordered subset of badges that apply to this phrase.
  ///
  /// It is better to ask the PhraseModel directly than to invoke this
  /// method (which must ascend the tree of phrases to find the model.
  Badges badges() const;

  /**\brief Convenience functions to fetch and modify content.
    */
  ///@{

  /// Return the title text that should be displayed.
  virtual std::string title() const
  {
    return m_content ? m_content->stringValue(PhraseContent::TITLE) : std::string();
  }
  /// Indicate whether users should be allowed to edit the title text.
  virtual bool isTitleMutable() const
  {
    return m_content ? m_content->editable(PhraseContent::TITLE) : false;
  }
  /// A method called by user interface code to change the title text, returning true on success.
  virtual bool setTitle(const std::string& newTitle)
  {
    return m_content ? m_content->editStringValue(PhraseContent::TITLE, newTitle) : false;
  }

  /// Return supplementary text that should be displayed with less emphasis than the title.
  virtual std::string subtitle()
  {
    return m_content ? m_content->stringValue(PhraseContent::SUBTITLE) : std::string();
  }
  /// Indicate whether users should be allowed to edit the subtitle text.
  virtual bool isSubtitleMutable() const
  {
    return m_content ? m_content->editable(PhraseContent::SUBTITLE) : false;
  }
  /// A method called by user interfaces to change the subtitle text, returning true on success.
  virtual bool setSubtitle(const std::string& newSubtitle)
  {
    return m_content ? m_content->editStringValue(PhraseContent::SUBTITLE, newSubtitle) : false;
  }

  /// Return the persistent object related to this phrase (or nullptr if not well defined).
  virtual smtk::resource::PersistentObjectPtr relatedObject() const
  {
    return m_content ? m_content->relatedObject() : smtk::resource::PersistentObjectPtr();
  }
  /// Return the resource related to this phrase (or nullptr if not well defined).
  virtual smtk::resource::ResourcePtr relatedResource() const
  {
    return m_content ? m_content->relatedResource() : smtk::resource::ResourcePtr();
  }
  /// Return the resource component related to this phrase (or nullptr if not well defined).
  virtual smtk::resource::ComponentPtr relatedComponent() const
  {
    return m_content ? m_content->relatedComponent() : smtk::resource::ComponentPtr();
  }
  /// A convenience function that returns the related component's UUID if one exists.
  virtual smtk::common::UUID relatedComponentId() const;

  /// If this phrase describes a property attached to a component, return the property name.
  virtual std::string relatedPropertyName() const
  {
    return std::string();
  } // TODO: Not exposed in content yet.
  /// If this phrase describes a property attached to a component, return the property's type.
  virtual smtk::resource::PropertyType relatedPropertyType() const
  {
    return smtk::resource::INVALID_PROPERTY;
  }

  ///@}

  /// Return the role that the phrase plays in the description.
  virtual DescriptivePhraseType phraseType() const { return m_type; }

  /// Return the parent phrase of this phrase (or null).
  virtual DescriptivePhrasePtr parent() const { return m_parent.lock(); }

  /// Indicate that this phrases children need to be rebuilt (or not if \a dirty is false).
  virtual void markDirty(bool dirty = true) { m_subphrasesBuilt = !dirty; }
  /// Indicate whether the list of subphrases is built by a subphrase generator and is up to date.
  virtual bool areSubphrasesBuilt() const { return m_subphrasesBuilt; }

  /// Return children phrases that further describe the subject of this phrase.
  virtual DescriptivePhrases& subphrases();
  /// Return children phrases that further describe the subject of this phrase.
  virtual DescriptivePhrases subphrases() const;

  /// Return the index of the given phrase in this instance's subphrases (or -1).
  virtual int argFindChild(const DescriptivePhrase* child) const;

  /**\brief Return the index of the given Resource pointer in this instance's subphrases (or -1).
    *
    * If \a onlyResource is true, this will only find children whose relatedResource()
    * matches \a child but whose relatedComponent() is null (meaning that the phrase is
    * describing the resource rather than a component of the resource).
    */
  virtual int argFindChild(const smtk::resource::ResourcePtr& child, bool onlyResource = true)
    const;
  /// Return the index of the given Component pointer in this instance's subphrases (or -1).
  virtual int argFindChild(const smtk::resource::ComponentPtr& child) const;

  /**\brief Return the index of the given property (name, type) in this instance's subphrases.
    *
    * This assumes that the property name exactly matches the title of the phrase.
    * When there is no match, -1 is returned.
    */
  virtual int argFindChild(const std::string& propName, smtk::resource::PropertyType propType)
    const;

  /// Return the location of this phrase in its parent's array of children or -1 (if no parent).
  int indexInParent() const;

  /**\brief Populate the index \a idx, from the top of the hierarchy, that will select this phrase.
    *
    * Descriptive phrases form a hierarchy, i.e., a tree.
    * Every phrase thus has an index \a p, specified as a vector
    * of integers that select the children from the root of the
    * hierarchy down to itself.
    * This method populates \a idx with the selector for this phrase.
    *
    * An empty \a idx indicates the current phrase is the
    * root of the hierarchy.
    *
    * When not empty, the first entry in the vector corresponds to the
    * immediate child of the root phrase to descend to reach this phrase.
    * The final entry in the returned vector \a p is
    * identical to the value of indexInParent() (which is also
    * identical to parent()->argFindChild(shared_from_this())).
    */
  void index(std::vector<int>& idx) const;
  /// A convenience method that returns the index for the phrase.
  std::vector<int> index() const
  {
    std::vector<int> idx;
    this->index(idx);
    return idx;
  }

  /// Return the root phrase of the tree containing this item.
  DescriptivePhrasePtr root() const;

  /**\brief Descend children of this phrase along the \a relativePath, returning the child or null.
    *
    * Negative entries in \a relativePath indicate that parents, rather than
    * children should be visited.
    * For example, a -2 indicates that the grandparent of the current phrase
    * should be visited.
    *
    * If a parent or child does not exist, a null pointer is returned.
    */
  DescriptivePhrasePtr relative(const std::vector<int>& relativePath) const;

  /**\brief Descend the root of the current phrase given the absolute path, returning the child or null.
    *
    * This is equivalent to this->root()->relative(absolutePath).
    * Negative entries may exist in \a absolutePath, but the first
    * entry must be non-negative.
    */
  DescriptivePhrasePtr at(const std::vector<int>& absolutePath) const;

  /// Return a unique integer ID of this phrase.
  unsigned int phraseId() const { return m_phraseId; }

  /// Find the subphrase generator for this phrase (held locally or by any parent) or return null.
  SubphraseGeneratorPtr findDelegate() const;
  /// Return true if this phrase's type is any of: {STRING, FLOAT, INTEGER}_PROPERTY_VALUE
  virtual bool isPropertyValueType() const;

  /**\brief Invoke \a fn on all of this phrase's **existing** children recursively.
    *
    * This method will not descend any phrase if areSubphrasesBuilt() returns false.
    *
    * The visitor \a fn should **never** modify any parents of the element
    * in the phrase hierarchy it is called upon or the indices it is passed
    * may become invalid. It is possible to modify the current element,
    * including methods that will cause its children to change.
    * In this case, the modified children will be visited since children
    * are visited after their parents.
    *
    * It is possible in the future that this method may execute in parallel.
    * If so, it will also accept a c++17 execution policy.
    */
  virtual void visitChildren(Visitor fn);

  /// Return the PhraseModel (obtained via the subphrase generator) holding this phrase (or null).
  PhraseModelPtr phraseModel() const;

  /**\brief Phrase-type-based comparison method for DescriptivePhrases
    *
    * This sorts DescriptivePhrases based first on their type and then by their
    * title text.
    */
  static bool compareByTypeThenTitle(const DescriptivePhrasePtr& a, const DescriptivePhrasePtr& b);

  /**\brief Title-based comparison method for DescriptivePhrases
    *
    * This can be used to help sort DescriptivePhrases based solely on their
    * titles.
    */
  static bool compareByTitle(const DescriptivePhrasePtr& a, const DescriptivePhrasePtr& b);

  /** \brief Provide contents-based comparison for phrases.
    *
    * This allows unordered sets and maps to hold phrases.
    * Also, it supports comparisons done by PhraseModel::updateChildren().
    *
    * These compare m_parent, m_type, and m_delegate but do not
    * examine subphrases or phrase ID. Comparing these 3 values
    * should be enough to prove that they will behave the same
    * if given the chance to prepare themselves.
    */
  bool operator==(const DescriptivePhrase& other) const;
  bool operator!=(const DescriptivePhrase& other) const;

  /// Do not call this; it is for use by subphrase generators.
  void reparent(const DescriptivePhrasePtr& nextParent);

protected:
  friend class SubphraseGenerator;

  DescriptivePhrase();

  /// Build (if required) the cached subphrases of this phrase.
  void buildSubphrases();

  /**\brief A special method for dealing with explicitly-specified lists.
    *
    * This method exists to be called by PhraseListContent::setup() in
    * order to populate the subphrases with a developer-specified list
    * of phrases instead of having a subphrase generator create them.
    *
    * This pattern is used indirectly by the SubphraseGenerator when
    * creating subphrases that should themselves be grouped into multiple
    * phrases rather than directly becoming children of a parent.
    * A concrete example is when a model's cells should be split into
    * a phrase listing faces and another listing edges.
    *
    * Ordinarily the subphrase generator should not be bypassed, but
    * here that creates problems because the subphrase generator may
    * not be able to construct the list of phrases. Also, it may not
    * be possible at this point to invoke methods on the PhraseModel
    * to register new child phrases because the grandchildren are inserted
    * into the child lists before the children are inserted into the
    * parent... this means that the insertion would be provided a
    * non-existent parent index.
    */
  void manuallySetSubphrases(const DescriptivePhrases& children, bool notify = false);

  /// Actual implementation of the public visitChildren() method.
  int visitChildrenInternal(Visitor fn, std::vector<int>& indices);

  WeakDescriptivePhrasePtr m_parent;
  DescriptivePhraseType m_type;
  SubphraseGeneratorPtr m_delegate;
  PhraseContentPtr m_content;
  unsigned int m_phraseId;
  mutable DescriptivePhrases m_subphrases;
  mutable bool m_subphrasesBuilt;

private:
  static unsigned int s_nextPhraseId;
};
} // namespace view
} // namespace smtk

#endif
