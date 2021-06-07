//========================================================================================
//  
//  $File: SelectionHelper.h $
//
//  Created by jae on 3/20/15
//
//  
//========================================================================================

#ifndef  __SelectionHelper_h__
#define  __SelectionHelper_h__

/** Shows how to select and deselect objects.

	@see AIArtSuite, AIMatchingArtSuite, SnpArtSetHelper
  	@ingroup sdk_snippet
 */
class SelectionHelper
{
	public:
		/** Checks if the user is currently working with a document.
			@return true if there is a current document, false otherwise.
			@see SnpDocumentHelper::GetCurrentDocument
		*/
		ASBoolean IsDocumentSelected(void);

		/** Checks if art objects are selected.
			@param artType IN specifies type of selected art to match (default kAnyArt)
			@return true if art objects are selected, false otherwise.
			@see SnpArtSetHelper
		*/
		ASBoolean IsArtSelected(AIArtType artType = kAnyArt);

		/** Checks if a range of text characters is selected.
			@return true if a range of text characters is selected, false otherwise.
		*/
		ASBoolean IsTextRangeSelected(void);

		/** Checks whether given art object can be selected - objects that are on layers 
			that are locked or hidden should not be selected.
			@param art IN object to select.
			@return true if given art object can be selected, false otherwise.
		*/
		ASBoolean CanSelectArt(AIArtHandle art);

		/** Select given art object.
			@param art IN object to select.
			@return kNoErr on success, other ASErr otherwise.
		*/
		ASErr SelectArt(AIArtHandle art);

		/** Deselects all art in the document the user is currently working with.
			@return kNoErr on success, other ASErr otherwise.
		*/
		ASErr DeselectAll();
    
        /** Get the selected text frame
            @return the selected text frame
         */
        ASErr GetSelectedTextFrame(AIArtHandle &frameArt);
    
        /** Get the selected object
            @return selected art handle
         */
        ASErr GetSelectedArtSet(AIArtHandle **arts, ai::int32 &numMatched);
};

#endif // __SelectionHelper_h__

// End SelectionHelper.h

