#ifndef __UPDATE_GD_H
#define __UPDATE_GD_H

#include <tsu/drawable.h>
#include "../gdemu_sdk.h"

class UpdateGD : public Drawable
{
  public:
	UpdateGD();
	virtual ~UpdateGD();

	virtual void draw(int list);
	char *getText() { return m_text; };

	void next();
	void prev();

  private:
	char *m_text;
};

#endif /* __UPDATE_GD_H */