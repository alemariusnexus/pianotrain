#ifndef GUIDOEXTENSIONS_H_
#define GUIDOEXTENSIONS_H_

#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <ostream>


//void MarkActiveNotes(ARHandler ar, int voicenum, int32_t num, int32_t denom);

void GuidoPrintAR(ARHandler ar, std::ostream& out);


#endif /* GUIDOEXTENSIONS_H_ */
