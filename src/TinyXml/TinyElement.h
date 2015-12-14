#ifndef AOS_TinyXML_TinyElement_H
#define AOS_TinyXML_TinyElement_H

/** The element is a container class. It has a value, the element name,
	and can contain other elements, text, comments, and unknowns.
	Elements also contain an arbitrary number of attributes.
*/
class TiXmlElement : public TiXmlNode
{
public:
	/// Construct an element.
	TiXmlElement (const char * in_value);

	#ifdef TIXML_USE_STL
	/// std::string constructor.
	TiXmlElement( const std::string& _value );
	#endif

	TiXmlElement( const TiXmlElement& );

	void operator=( const TiXmlElement& base );

	virtual ~TiXmlElement();

	// Given an attribute name, Attribute() returns the value
	// for the attribute of that name, or null if none exists.
	const char* Attribute( const char* name ) const;

	// Given an attribute name, Attribute() returns the value
	//	for the attribute of that name, or null if none exists.
	//	If the attribute exists and can be converted to an integer,
	//	the integer value will be put in the return 'i', if 'i'
	//	is non-null.
	const char* Attribute( const char* name, int* i ) const;

	//  Given an attribute name, Attribute() returns the value
	//	for the attribute of that name, or null if none exists.
	//	If the attribute exists and can be converted to an double,
	//	the double value will be put in the return 'd', if 'd'
	//	is non-null.
	const char* Attribute( const char* name, double* d ) const;

	//  QueryIntAttribute examines the attribute - it is an alternative to the
	//	Attribute() method with richer error checking.
	//	If the attribute is an integer, it is stored in 'value' and 
	//	the call returns TIXML_SUCCESS. If it is not
	//	an integer, it returns TIXML_WRONG_TYPE. If the attribute
	//	does not exist, then TIXML_NO_ATTRIBUTE is returned.
	int QueryIntAttribute( const char* name, int* _value ) const;
	int QueryDoubleAttribute( const char* name, double* _value ) const;
	int QueryFloatAttribute( const char* name, float* _value ) const {
		double d;
		int result = QueryDoubleAttribute( name, &d );
		if ( result == TIXML_SUCCESS ) {
			*_value = (float)d;
		}
		return result;
	}
    #ifdef TIXML_USE_STL
	/** Template form of the attribute query which will try to read the
		attribute into the specified type. Very easy, very powerful, but
		be careful to make sure to call this with the correct type.

		@return TIXML_SUCCESS, TIXML_WRONG_TYPE, or TIXML_NO_ATTRIBUTE
	*/
	template< typename T > int QueryValueAttribute( 
			const std::string& name, T* outValue ) const
	{
		const TiXmlAttribute* node = attributeSet.Find( name );
		if ( !node )
			return TIXML_NO_ATTRIBUTE;

		std::stringstream sstream( node->ValueStr() );
		sstream >> *outValue;
		if ( !sstream.fail() )
			return TIXML_SUCCESS;
		return TIXML_WRONG_TYPE;
	}
	#endif

	//  Sets an attribute of name to a given value. The attribute
	//	will be created if it does not exist, or changed if it does.
	void SetAttribute( const char* name, const char * _value );

    #ifdef TIXML_USE_STL
	const std::string* Attribute( const std::string& name ) const;
	const std::string* Attribute( const std::string& name, int* i ) const;
	const std::string* Attribute( const std::string& name, double* d ) const;
	int QueryIntAttribute( const std::string& name, int* _value ) const;
	int QueryDoubleAttribute( const std::string& name, double* _value ) const;

	// STL std::string form.
	void SetAttribute( const std::string& name, const std::string& _value );
	//< STL std::string form.
	void SetAttribute( const std::string& name, int _value );
	#endif

	//  Sets an attribute of name to a given value. The attribute
	//	will be created if it does not exist, or changed if it does.
	void SetAttribute( const char * name, int value );

	//  Sets an attribute of name to a given value. The attribute
	//	will be created if it does not exist, or changed if it does.
	void SetDoubleAttribute( const char * name, double value );

	//  Deletes an attribute with the given name.
	void RemoveAttribute( const char * name );
    #ifdef TIXML_USE_STL
	void RemoveAttribute( const std::string& name )	{	RemoveAttribute (name.c_str ());	}	///< STL std::string form.
	#endif

	const TiXmlAttribute* FirstAttribute() const	{ return attributeSet.First(); }
	TiXmlAttribute* FirstAttribute() 				{ return attributeSet.First(); }
	const TiXmlAttribute* LastAttribute()	const 	{ return attributeSet.Last(); }
	TiXmlAttribute* LastAttribute()					{ return attributeSet.Last(); }

	//  Convenience function for easy access to the text inside an element. 
	//  Although easy and concise, GetText() is limited compared to getting the 
	//  TiXmlText child and accessing it directly.
	//	
	//	If the first child of 'this' is a TiXmlText, the GetText()
	//	returns the character string of the Text node, else null is returned.
	//
	//	This is a convenient method for getting the text of simple contained text:
	//	@verbatim
	//	<foo>This is text</foo>
	//	const char* str = fooElement->GetText();
	//	@endverbatim
	//
	//	'str' will be a pointer to "This is text". 
	//	
	//	Note that this function can be misleading. If the element foo was created from
	//	this XML:
	//	@verbatim
	//	<foo><b>This is text</b></foo> 
	//	@endverbatim
	//
	//	then the value of str would be null. The first child node isn't a text node, 
	//	it is another element. From this XML:
	//	@verbatim
	//	<foo>This is <b>text</b></foo> 
	//	@endverbatim
	//	GetText() will return "This is ".
	//
	//	WARNING: GetText() accesses a child node - don't become confused with the 
	//			 similarly named TiXmlHandle::Text() and TiXmlNode::ToText() which are 
	//			 safe type casts on the referenced node.
	const char* GetText() const;

	/// Creates a new Element and returns it - the returned element is a copy.
	virtual TiXmlNode* Clone() const;
	// Print the Element to a FILE stream.
	virtual void Print( FILE* cfile, int depth ) const;

	//	Attribtue parsing starts: next char past '<'
	//					 returns: next char past '>'
	virtual const char* Parse( 
			const char* p, 
			TiXmlParsingData* data, 
			TiXmlEncoding encoding );

	///< Cast to a more defined type. Will return null not of the requested type.
	virtual const TiXmlElement* ToElement() const { return this; } 
	virtual TiXmlElement* ToElement() { return this; } 

	// Walk the XML tree visiting this node and all of its children. 
	virtual bool Accept( TiXmlVisitor* visitor ) const;

	// Chen Ding, 02/21/2009
	const char *getNodeAttribute(const char *path, const char *dft_value);
	const char *Attribute(const char *name, const char *dft_value);
	bool		removeAttribute(const char *path);
	bool		setText(const char *value);

protected:
	void CopyTo( TiXmlElement* target ) const;
	void ClearThis();	// like clear, but initializes 'this' object as well

	// Used to be public [internal use]
	#ifdef TIXML_USE_STL
	virtual void StreamIn( std::istream * in, TIXML_STRING * tag );
	#endif
	//	[internal use]
	//	Reads the "value" of the element -- another element, or text.
	//	This should terminate with the current end tag.
	const char* ReadValue(const char* in, 
			TiXmlParsingData* prevData, 
			TiXmlEncoding encoding );

private:
	TiXmlAttributeSet attributeSet;
};

#endif
