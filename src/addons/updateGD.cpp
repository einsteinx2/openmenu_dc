#include "updateGD.h"

UpdateGD::UpdateGD()
{
}
UpdateGD::~UpdateGD()
{
}

void UpdateGD::draw(int list)
{
}

void UpdateGD::next()
{
    gdemu_img_cmd(GDEMU_NEXT_IMG);
}

void UpdateGD::prev()
{
    gdemu_img_cmd(GDEMU_PREV_IMG);
}