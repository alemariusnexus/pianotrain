#ifndef GUIDONOTEMARKER_H_
#define GUIDONOTEMARKER_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>
#include <functional>

// TODO: Get rid of this header here...
#include <Guido/abstract/Fraction.h>


// Forward declarations so that the headers don't depend on Guidolib's internal headers

class ARMusicalVoice;
class ARMusicalObject;
class ARNote;

//class Fraction;
typedef Fraction TYPE_TIMEPOSITION;

typedef void *			GuidoPos;




class GuidoNoteMarker : public QObject
{
	Q_OBJECT

public:
	GuidoNoteMarker(ARHandler ar, QObject* parent = nullptr);

	bool setPerformanceMarker(int32_t num, int32_t denom);
	void clearPerformanceMarker();

	bool markCorrectPlay(int8_t midiKey, int32_t num, int32_t denom);
	void clearCorrectPlayMarkers();

	bool markMissedNote(int8_t midiKey, int32_t num, int32_t denom);
	void clearMissedNoteMarkers();

	void clear();

	void setPerformanceMarkerColors(const QColor& noteColor, const QColor& restColor);
	void setCorrectPlayColor(const QColor& color);
	void setMissedNoteColor(const QColor& color);

private:
	bool setPerformanceMarker(ARMusicalVoice* voice, int32_t num, int32_t denom);

	bool markSingleNote(int8_t midiKey, int32_t num, int32_t denom, const QColor& color, QList<ARMusicalObject*>& formatList);
	bool markSingleNote(ARMusicalVoice* voice, int8_t midiKey, int32_t num, int32_t denom, const QColor& color, QList<ARMusicalObject*>& formatList);

	//bool markCorrectPlay(ARMusicalVoice* voice, int8_t midiKey, int32_t num, int32_t denom);

	//bool markMissedNote(ARMusicalVoice* voice, int8_t midiKey, int32_t num, int32_t denom);

	bool findTagBounds (
			TYPE_TIMEPOSITION& startTimepos, TYPE_TIMEPOSITION& endTimepos,
			GuidoPos& startPos, GuidoPos& endPos,
			ARMusicalVoice* voice,
			const std::function<bool(ARMusicalObject*, TYPE_TIMEPOSITION, GuidoPos, void*)>& startFilter,
			const std::function<bool(ARMusicalObject*, TYPE_TIMEPOSITION, GuidoPos, void*)>& endFilter,
			bool chordMode = false,
			void* state = nullptr
			);

	bool findNoteTagBounds (
			TYPE_TIMEPOSITION& startTimepos, TYPE_TIMEPOSITION& endTimepos,
			GuidoPos& startPos, GuidoPos& endPos,
			ARMusicalVoice* voice,
			const std::function<bool(ARNote*, TYPE_TIMEPOSITION)>& filter,
			bool chordMode = false
			);

	void formatNotes (
			TYPE_TIMEPOSITION startTimepos, TYPE_TIMEPOSITION endTimepos,
			GuidoPos startPos, GuidoPos endPos,
			ARMusicalVoice* voice,
			const QColor& color,
			QList<ARMusicalObject*>& formatList
			);

	void formatRests (
			TYPE_TIMEPOSITION startTimepos, TYPE_TIMEPOSITION endTimepos,
			GuidoPos startPos, GuidoPos endPos,
			ARMusicalVoice* voice,
			const QColor& color,
			QList<ARMusicalObject*>& formatList
			);

	QString buildGuidoColorString(const QColor& color) const;

private:
	ARHandler ar;

	QColor perfMarkerNoteColor;
	QColor perfMarkerRestColor;

	TYPE_TIMEPOSITION curPerfMarkerStartTimepos;
	TYPE_TIMEPOSITION curPerfMarkerEndTimepos;

	QColor corrPlayColor;
	QColor missedNoteColor;

	QList<ARMusicalObject*> curPerfMarkerObjs;
	QList<ARMusicalObject*> corrPlayMarkerObjs;
	QList<ARMusicalObject*> missedNoteMarkerObjs;
};

#endif /* GUIDONOTEMARKER_H_ */
