#ifndef GUIDONOTEMARKER_H_
#define GUIDONOTEMARKER_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <Guido/GUIDOParse.h>
#include <Guido/GUIDOEngine.h>


class ARMusicalVoice;
class ARMusicalObject;


class GuidoNoteMarker : public QObject
{
	Q_OBJECT

public:
	GuidoNoteMarker(ARHandler ar, QObject* parent = nullptr);

	void setPerformanceMarker(int32_t num, int32_t denom);
	void clearPerformanceMarker();

	void setPerformanceMarkerColors(const QColor& noteColor, const QColor& restColor);

private:
	void setPerformanceMarker(ARMusicalVoice* voice, int32_t num, int32_t denom);

	QString buildGuidoColorString(const QColor& color) const;

private:
	ARHandler ar;

	QColor perfMarkerNoteColor;
	QColor perfMarkerRestColor;

	QList<ARMusicalObject*> curPerfMarkerObjs;
};

#endif /* GUIDONOTEMARKER_H_ */
