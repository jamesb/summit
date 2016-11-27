#include <pebble.h>
#include "misc.h"



/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html and from
/// http://c-faq.com/misc/intovf.html
/// 
/// @param[in,out]  sum  Pointer will equal the sum of the addition, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      augend  first number to add
/// @param[in]      addend  second number to add
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the sum pointer is NULL.
///          MPA_OVERFLOW_ERR if the addition would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode u16add_u16_u16(uint16_t *sum, const uint16_t augend, const uint16_t addend) {
  MPA_RETURN_IF_NULL(sum);
  if (MPA_MAX(uint16_t) - addend < augend)  return MPA_OVERFLOW_ERR;
	*sum = augend + addend;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html and from
/// http://c-faq.com/misc/intovf.html
/// 
/// @param[in,out]  sum  Pointer will equal the sum of the addition, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      augend  first number to add
/// @param[in]      addend  second number to add
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the sum pointer is NULL.
///          MPA_OVERFLOW_ERR if the addition would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode u16add_u16_s16(uint16_t *sum, const uint16_t augend, const int16_t addend) {
  MPA_RETURN_IF_NULL(sum);
  if (addend < 0) {
    if (MPA_MIN(uint16_t) - addend > augend)  return MPA_OVERFLOW_ERR;
  } else {
    if (MPA_MAX(uint16_t) - addend < augend)  return MPA_OVERFLOW_ERR;
  }
	*sum = augend + addend;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html and from
/// http://c-faq.com/misc/intovf.html
/// 
/// @param[in,out]  sum  Pointer will equal the sum of the addition, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      augend  first number to add
/// @param[in]      addend  second number to add
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the sum pointer is NULL.
///          MPA_OVERFLOW_ERR if the addition would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode s16add_u16_u16(int16_t *sum, const uint16_t augend, const uint16_t addend) {
  MPA_RETURN_IF_NULL(sum);
  if (MPA_MAX(int16_t) - addend < augend)  return MPA_OVERFLOW_ERR;
	*sum = augend + addend;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html and from
/// http://c-faq.com/misc/intovf.html
/// 
/// @param[in,out]  sum  Pointer will equal the sum of the addition, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      augend  first number to add
/// @param[in]      addend  second number to add
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the sum pointer is NULL.
///          MPA_OVERFLOW_ERR if the addition would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode s16add_u16_s16(int16_t *sum, const uint16_t augend, const int16_t addend) {
  MPA_RETURN_IF_NULL(sum);
  if (addend < 0) {
    if (MPA_MIN(int16_t) - addend > augend)  return MPA_OVERFLOW_ERR;
  } else {
    if (MPA_MAX(int16_t) - addend < augend)  return MPA_OVERFLOW_ERR;
  }
	*sum = augend + addend;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html and from
/// http://c-faq.com/misc/intovf.html
/// 
/// @param[in,out]  sum  Pointer will equal the sum of the addition, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      augend  first number to add
/// @param[in]      addend  second number to add
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the sum pointer is NULL.
///          MPA_OVERFLOW_ERR if the addition would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode u32add_u32_u32(uint32_t *sum, const uint32_t augend, const uint32_t addend) {
  MPA_RETURN_IF_NULL(sum);
  if (MPA_MAX(uint32_t) - addend < augend)  return MPA_OVERFLOW_ERR;
	*sum = augend + addend;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html and from
/// http://c-faq.com/misc/intovf.html
/// 
/// @param[in,out]  sum  Pointer will equal the sum of the addition, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      augend  first number to add
/// @param[in]      addend  second number to add
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the sum pointer is NULL.
///          MPA_OVERFLOW_ERR if the addition would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode u32add_u32_s32(uint32_t *sum, const uint32_t augend, const int32_t addend) {
  MPA_RETURN_IF_NULL(sum);
  if (addend < 0) {
    if(MPA_MIN(uint32_t) - addend > augend)  return MPA_OVERFLOW_ERR;
  } else {
    if(MPA_MAX(uint32_t) - addend < augend)  return MPA_OVERFLOW_ERR;
  }
	*sum = augend + addend;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html and from
/// http://c-faq.com/misc/intovf.html
/// 
/// @param[in,out]  sum  Pointer will equal the sum of the addition, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      augend  first number to add
/// @param[in]      addend  second number to add
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the sum pointer is NULL.
///          MPA_OVERFLOW_ERR if the addition would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode s32add_u32_u32(int32_t *sum, const uint32_t augend, const uint32_t addend) {
  MPA_RETURN_IF_NULL(sum);
  if (MPA_MAX(int32_t) - addend < augend)  return MPA_OVERFLOW_ERR;
	*sum = augend + addend;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html and from
/// http://c-faq.com/misc/intovf.html
/// 
/// @param[in,out]  sum  Pointer will equal the sum of the addition, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      augend  first number to add
/// @param[in]      addend  second number to add
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the sum pointer is NULL.
///          MPA_OVERFLOW_ERR if the addition would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode s32add_u32_s32(int32_t *sum, const uint32_t augend, const int32_t addend) {
  MPA_RETURN_IF_NULL(sum);
  if (addend < 0) {
    // JRB NOTE: Compiler warns about comparing signed and unsigned if the
    // first casts in these comparisons are int32_t.
    if((uint32_t)(MPA_MIN(int32_t) - addend) > augend)  return MPA_OVERFLOW_ERR;
  } else {
    if((uint32_t)(MPA_MAX(int32_t) - addend) < augend)  return MPA_OVERFLOW_ERR;
  }
	*sum = augend + addend;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing an addition operation.
/// This code largely borrowed from https://www.fefe.de/intof.html and from
/// http://c-faq.com/misc/intovf.html
/// 
/// @param[in,out]  sum  Pointer will equal the sum of the addition, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      augend  first number to add
/// @param[in]      addend  second number to add
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the sum pointer is NULL.
///          MPA_OVERFLOW_ERR if the addition would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode s32add_s32_s32(int32_t* sum, const int32_t augend, const int32_t addend) {
  MPA_RETURN_IF_NULL(sum);
  if (addend < 0) {
    if((int32_t)(MPA_MIN(int32_t) - addend) > augend)  return MPA_OVERFLOW_ERR;
  } else {
    if((int32_t)(MPA_MAX(int32_t) - addend) < augend)  return MPA_OVERFLOW_ERR;
  }
	*sum = augend + addend;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing a multiplication operation.
/// This code largely borrowed from https://www.fefe.de/intof.html
/// 
/// @param[in,out]  product  The variable that will hold the product, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      multiplicand  first number to multiply
/// @param[in]      multiplier  second number to multiply
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the product pointer is NULL.
///          MPA_OVERFLOW_ERR if the multiplication would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode u32mult_u32_u32(uint32_t* product, const uint32_t multiplicand, const uint32_t multiplier) {
  MPA_RETURN_IF_NULL(product);
  uint64_t x=(uint64_t)multiplicand*multiplier;
  if (x>0xffffffff) return MPA_OVERFLOW_ERR;
  *product=x&0xffffffff;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Checks for integer overflow before performing a multiplication operation.
/// This code largely borrowed from: 
/// https://www.securecoding.cert.org/confluence/display/c/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow
/// 
/// @param[in,out]  product  The variable that will hold the product, if no
///       overflow would occur. Upon entry to this function, this must point
///       to a valid variable.
/// @param[in]      multiplicand  first number to multiply
/// @param[in]      multiplier  second number to multiply
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the product pointer is NULL.
///          MPA_OVERFLOW_ERR if the multiplication would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode s32mult_s32_s32(int32_t* product, const int32_t multiplicand, const int32_t multiplier) {
  MPA_RETURN_IF_NULL(product);
  if (multiplicand > 0) {  /* multiplicand is positive */
    if (multiplier > 0) {  /* multiplicand and multiplier are positive */
      if (multiplicand > (MPA_MAX(int32_t) / multiplier)) return MPA_OVERFLOW_ERR;
    } else { /* multiplicand positive, multiplier nonpositive */
      if (multiplier < (MPA_MIN(int32_t) / multiplicand)) return MPA_OVERFLOW_ERR;
    } /* multiplicand positive, multiplier nonpositive */
  } else { /* multiplicand is nonpositive */
    if (multiplier > 0) { /* multiplicand is nonpositive, multiplier is positive */
      if (multiplicand < (MPA_MIN(int32_t) / multiplier)) return MPA_OVERFLOW_ERR;
    } else { /* multiplicand and multiplier are nonpositive */
      if ( (multiplicand != 0) && (multiplier < (MPA_MAX(int32_t) / multiplicand))) return MPA_OVERFLOW_ERR;
    } /* End if multiplicand and multiplier are nonpositive */
  } /* End if multiplicand is nonpositive */
 
  *product = multiplicand * multiplier;
  return MPA_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
/// Rounds up to the value nearest the specified multiple. No floating point
/// math is used.
/// This code largely borrowed from: 
/// http://stackoverflow.com/questions/3407012/c-rounding-up-to-the-nearest-multiple-of-a-number
/// 
/// @param[in,out]  rounded  The variable that will hold the rounded result
///       Upon entry to this function, this must point to a valid variable.
/// @param[in]      numToRound  original number to be rounded
/// @param[in]      multiple  specifies that the rounding operation should
///       result in the nearest multiple of this value that is >= the number
///       to be rounded
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the rounded pointer is NULL.
///          MPA_OVERFLOW_ERR if the operation would cause an integer overflow
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode s32RoundUp(int32_t* rounded, const int32_t numToRound, const int32_t multiple) {
  MPA_RETURN_IF_NULL(rounded);
  MagPebApp_ErrCode mpaRet = MPA_SUCCESS;
  if (multiple == 0) *rounded = numToRound;

  int32_t remainder = abs(numToRound) % multiple;
  if (remainder == 0) *rounded = numToRound;

  if (numToRound < 0) {
    int32_t temp = abs(numToRound);
    if ( (mpaRet = s32add_s32_s32(&temp, temp, -remainder)) != MPA_SUCCESS) return mpaRet;
    *rounded = temp;
  } else {
    int32_t temp = numToRound;
    if ( (mpaRet = s32add_s32_s32(&temp, temp, multiple)) != MPA_SUCCESS) return mpaRet;
    if ( (mpaRet = s32add_s32_s32(&temp, temp, -remainder)) != MPA_SUCCESS) return mpaRet;
    *rounded = temp;
  }
  return mpaRet;
}


/////////////////////////////////////////////////////////////////////////////
/// Stylizes the text layer to the spec
/////////////////////////////////////////////////////////////////////////////
void textLayer_stylize(TextLayer* textLayer, const GColor bgcolor, const GColor txtColor,
                       const GTextAlignment txtAlign, const GFont txtFont) {
  if (!textLayer) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Attempting to stylize before text layer is created!");
  } else {
    text_layer_set_background_color(textLayer, bgcolor);
    text_layer_set_text_color(textLayer, txtColor);
    text_layer_set_text_alignment(textLayer, txtAlign);
    text_layer_set_font(textLayer, txtFont);
  }
}


/////////////////////////////////////////////////////////////////////////////
/// Helper function for calculating relative coordinates.
/////////////////////////////////////////////////////////////////////////////
uint16_t rel2Pxl(const uint16_t max, const uint8_t pct) {
  return (pct * max) / 100;
}


/////////////////////////////////////////////////////////////////////////////
/// Copies a string with error handling and logging.
/////////////////////////////////////////////////////////////////////////////
bool strxcpy(char* dest, size_t bufSize, const char* src, const char* readable) {
  if (src) {
    long ret = 0;
    if ((ret = snprintf(dest, bufSize, "%s", src)) < 0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "%s string output error. snprintf=%ld", readable, ret);
      return false;
    } else if ((size_t)ret >= bufSize) {
      APP_LOG(APP_LOG_LEVEL_WARNING, "%s string was truncated. %ld characters required.", readable, ret);
    }
    return true;
  }
  return false;
}


/////////////////////////////////////////////////////////////////////////////
/// Copies a string from src to dest, where these strings may be NULL or
/// allocated, and (*dest) will result in a NULL or will be allocated to fit
/// the src string.
///
/// - If (*dest) is NULL, it will malloc'd to an appropriate size to contain
///   the string in src.
/// - If (*dest) is non-NULL, it will be realloc'd to an appropriate size to
///   contain the string in src.
/// - If src is NULL, then (*dest) will be freed and made NULL (or dest will
///   remain NULL if it is already NULL.
///
/// @param[in]      src  The source string. May be NULL, or it may be a
///       previously-allocated string, or a string literal. If it is a
///       string, it must be null-terminated.
/// @param[in,out]  dest  Pointer to the destination char*. The char* may be
///       NULL, or it may be a previously-allocated string. If it is a
///       string, it must be null-terminated upon entry. Upon exit, the
///       char* pointed to by dest is guaranteed to be either NULL or
///       null-terminated. <em>If the char* pointed to by dest is not NULL
///       on exit, then the caller is responsible for freeing it.</em>
///
/// @return  MPA_SUCCESS on success
///          MPA_NULL_POINTER_ERR if the dest double-pointer is NULL.
///          MPA_OUT_OF_MEMORY_ERR if memory could not be allocated for dest
/////////////////////////////////////////////////////////////////////////////
MagPebApp_ErrCode strxcpyalloc(char** dest, const char* src) {
  MPA_RETURN_IF_NULL(dest);

  size_t srcSize = strxlen(src) + 1;
  char* buffer = NULL;

  if (src == NULL) {
    if ((*dest) != NULL) {
      free((*dest));
      (*dest) = NULL;
    } else {
      // src == (*dest) == NULL
      return MPA_SUCCESS;
    }
  } else {
    // src parameter is not NULL
    if ((*dest) == NULL) {
      if ( (buffer = calloc(srcSize, sizeof(*buffer))) == NULL) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Insufficient memory");
        goto freemem;
      }
    } else {
      if ( (buffer = realloc((*dest), srcSize)) == NULL) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Insufficient memory");
        goto freemem;
      }
    }
    // buffer is allocated to the proper size and (*dest)
    // is either NULL or reallocated
    (*dest) = buffer;
    if (!strxcpy((*dest), srcSize, src, NULL)) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "strxcpy failed");
      goto freemem;
    }
  }

  return MPA_SUCCESS;


freemem:
  APP_LOG(APP_LOG_LEVEL_ERROR, "Error... freeing memory");
  if ((*dest) != NULL) {
    free((*dest));
    (*dest) = NULL;
  }
  return MPA_OUT_OF_MEMORY_ERR;
}


