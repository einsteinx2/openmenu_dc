#include <kos.h>
#include <math.h>
#include <tsu/genmenu.h>
#include <tsu/font.h>
#include <tsu/texture.h>
#include <tsu/drawables/banner.h>
#include <tsu/drawables/label.h>
#include <tsu/anims/logxymover.h>
#include <tsu/anims/expxymover.h>
#include <tsu/anims/alphafader.h>
#include <tsu/triggers/death.h>

extern "C"
{
#include "addons/pvr-texture.h"
}

extern uint8 romdisk[];
KOS_INIT_FLAGS(INIT_DEFAULT);
KOS_INIT_ROMDISK(romdisk);

class MyMenu : public GenericMenu, public RefCnt
{
  public:
    MyMenu(Font *fnt)
    {
        // Offset our scene so 0,0,0 is the screen center with Z +10
        m_scene->setTranslate(Vector(320, 240, 10));

        // Set a green background
        setBg(0.2f, 0.4f, 0.2f);

        m_white = Color(1, 1, 1, 1);
        m_gray = Color(1, 0.7f, 0.7f, 0.7f);

        int i;
        for (i = 0; i < 5; i++)
        {
            // Setup three labels and have them zoom in.
            m_options[i] = new Label(fnt, "DISC_TITLE", 24, true, true);
            m_options[i]->setTranslate(Vector(0, 400 + (i * 400), 0));
            m_options[i]->animAdd(new LogXYMover(0, -160 + 30 * i));
            m_options[i]->setTint(m_white);
            m_scene->subAdd(m_options[i]);
        }

        m_options[5] = new Label(fnt, "Quit", 24, true, true);
        m_options[5]->setTranslate(Vector(0, 400 + (5 * 400), 0));
        m_options[5]->animAdd(new LogXYMover(0, -160 + 30 *5));
        m_options[5]->setTint(m_white);
        m_scene->subAdd(m_options[5]);

        lbl2 = new Label(fnt, "openMenu Loader", 24, true, true);
        lbl2->setTranslate(Vector(0, -220, 0));
        lbl2->setTint(m_gray);
        m_scene->subAdd(lbl2);

        // Load a texture for our banner
        txr = new Texture();
        plx_texture_t *tex = *txr;
        pvr_ptr_t tl = TextureLoadPVR("/rd/0GDTEX.PVR", 0, 0);
        if (tl != (unsigned int*)-1)
        {
            tex->ptr = tl;
        }

        // Setup a scene and place a banner in it
        /*b = new Banner(PVR_LIST_TR_POLY, txr);
        b->setTranslate(Vector(0, 0, 0));
        m_scene->subAdd(b);*/
        m_cursel = 0;
    }

    virtual ~MyMenu()
    {
    }

    virtual void inputEvent(const Event &evt)
    {
        if (evt.type != Event::EvtKeypress)
            return;

        switch (evt.key)
        {
        case Event::KeyUp:
            m_cursel--;

            if (m_cursel < 0)
                m_cursel += 6;

            break;
        case Event::KeyDown:
            m_cursel++;

            if (m_cursel >= 6)
                m_cursel -= 6;

            break;
        case Event::KeySelect:
            printf("user selected option %d\n", m_cursel);

            if (m_cursel == 5)
                startExit();

            break;
        default:
            break;
        }

        for (int i = 0; i < 5; i++)
        {
            if (i == m_cursel)
                m_options[i]->setTint(m_white);
            else
                m_options[i]->setTint(m_gray);
        }
    }

    virtual void startExit()
    {
        // Apply some expmovers to the options.
        ExpXYMover *m = new ExpXYMover(0, 1, 0, 400);
        m->triggerAdd(new Death());
        m_options[0]->animAdd(m);

        m = new ExpXYMover(0, 1.2, 0, 400);
        m->triggerAdd(new Death());
        m_options[1]->animAdd(m);

        m = new ExpXYMover(0, 1.4, 0, 400);
        m->triggerAdd(new Death());
        m_options[2]->animAdd(m);
        GenericMenu::startExit();
    }

    Color m_white, m_gray;
    RefPtr<Label> m_options[6];
    RefPtr<Label> lbl2;
    RefPtr<Texture> txr;
    RefPtr<Banner> b;
    int m_cursel;
};

int main(int argc, char **argv)
{
    int done = 0, done2 = 0;

    // Guard against an untoward exit during testing.
    cont_btn_callback(0, CONT_START | CONT_A | CONT_B | CONT_X | CONT_Y,
                      (void (*)(unsigned char, long unsigned int))arch_exit);

    // Get 3D going
    pvr_init_defaults();

    // Load a font
    RefPtr<Font> fnt = new Font("/rd/revenant.txf");

    // Create a menu
    RefPtr<MyMenu> mm = new MyMenu(fnt);

    // Do the menu
    mm->doMenu();

    arch_exit();
    // Ok, we're all done! The RefPtrs will take care of mem cleanup.

    return 0;
}
