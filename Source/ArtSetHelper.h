//========================================================================================
//  
//  $File: ArtSetHelper.h $
//
//  Created by jae on 3/20/15
//
//  
//========================================================================================

#ifndef __ArtSetHelper_h__
#define __ArtSetHelper_h__

/** Helper class for working with art sets (see AIArtSetSuite) -
	the class allows art objects in documents to be found by art
	type (see AIArtType).

	@see AIArtSetSuite
  	@ingroup sdk_
*/
class ArtSetHelper {
public:
	/** Default constructor creates a new, empty art set.
		@see AIArtSetSuite::NewArtSet
	*/
	ArtSetHelper();

	/** Constructor finds art matching the given criteria in the current document.
		@param specs pointer to array sepcifying the type of art wanted.
		@param numSpecs the length of the specs array.
		@see AIArtSetSuite::NewArtSet
	*/
	ArtSetHelper(AIArtSpec* specs, short numSpecs);

	/** Copy constructor - copies given art set into this art set -
		current content of this art set is lost.
		@param rhs
	*/
	ArtSetHelper(const ArtSetHelper& rhs);

	/** Destructor - disposes of the art set.
		@see AIArtSetSuite::DisposeArtSet
	*/
	virtual ~ArtSetHelper();

	/** Assignment operator - disposes of the art set this
		object contains and creates a new art set that is 
		a copy of the given art set.
		@param IN ArtSetHelper object to assign to lhs.
		@return copy of ArtSetHelper passed in.
	*/
	ArtSetHelper& operator=(const ArtSetHelper& rhs);

	/** Index operator
		@param index IN 
		@return art handle at given index or NULL if index is invalid
	*/
	AIArtHandle operator [](size_t index) const;

	/** Gets number of art objects in this art set.
		@return number of art objects.
	*/
	size_t GetCount(void) const;

	/** Finds set of art that is selected - the current content
		of this art set is lost.
	*/
	void FindSelectedArt(void);

	/** Finds set of art in the given layer - the current content
		of this art set is lost.
		@param layerName IN name of layer art is on.
	*/
	void FindLayerArt(const ai::UnicodeString& layerName);

	/** Returns a new art set containing objects in this art set
		that match the given type.
		@param include IN the type of art to filter for (i.e. include)
		@return a new art set containing matching objects in this art set
	*/
	ArtSetHelper Filter(AIArtType include);

	/** Adds an object to this set.
		@param art IN the art object to add.
	*/
	void Add(AIArtHandle art);

	/** Selects all art objects in this art set.
	*/
	void SelectAll();

	/** Selects a given art object.
		@param art IN the art object to select.
	*/
	void SelectArt(AIArtHandle art);

	/** Gets this object's art set.
		@return the art set.
	*/
	inline AIArtSet GetArtSet(void) const {return fArtSet;}
    
    /** clear all art objects in this art set.
     */
    void ClearAll();



private:
	/** The art set associated with the instance of this class.
	*/
	AIArtSet fArtSet;
};

#endif

// End ArtSetHelper.h

