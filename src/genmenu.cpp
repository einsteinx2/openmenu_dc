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
#include "lowlevel/cdrom.h"
}

#include "addons/updateGD.h"

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

        m_red = Color(1, 1, 0.42f, 0.38f);

        m_options[0] = new Label(fnt, "Prev", 24, true, true);
        m_options[0]->setTranslate(Vector(0, 400, 0));
        m_options[0]->animAdd(new LogXYMover(0, -160));
        m_options[0]->setTint(m_white);
        m_scene->subAdd(m_options[0]);

        m_options[1] = new Label(fnt, "Next", 24, true, true);
        m_options[1]->setTranslate(Vector(0, 400 + 400, 0));
        m_options[1]->animAdd(new LogXYMover(0, -160 + 30));
        m_options[1]->setTint(m_gray);
        m_scene->subAdd(m_options[1]);

        m_options[2] = new Label(fnt, "-------", 24, true, true);
        m_options[2]->setTranslate(Vector(0, 400 + 400 + 400, 0));
        m_options[2]->animAdd(new LogXYMover(0, -160 + 60));
        m_options[2]->setTint(m_gray);
        m_scene->subAdd(m_options[2]);

        m_options[3] = new Label(fnt, "Quit", 24, true, true);
        m_options[3]->setTranslate(Vector(0, 400 + (3 * 400), 0));
        m_options[3]->animAdd(new LogXYMover(0, -160 + 30 * 3));
        m_options[3]->setTint(m_gray);
        m_scene->subAdd(m_options[3]);

        title_label = new Label(fnt, "openMenu Loader", 24, true, true);
        title_label->setTranslate(Vector(0, -220, 0));
        title_label->setTint(m_gray);
        m_scene->subAdd(title_label);

        disc_label = new Label(fnt, "DISC_TITLE", 24, true, true);
        disc_label->setTranslate(Vector(0, -20, 0));
        disc_label->setTint(m_gray);
        m_scene->subAdd(disc_label);

        status_label = new Label(fnt, "Welcome to openMenu!", 24, true, true);
        status_label->setTranslate(Vector(0, 220, 0));
        status_label->setTint(m_red);
        m_scene->subAdd(status_label);

        updateGD = new UpdateGD();
        m_scene->subAdd(updateGD);

        // Load a texture for our banner
        /*txr = new Texture();
        plx_texture_t *tex = *txr;
        pvr_ptr_t tl = TextureLoadPVR("/rd/0GDTEX.PVR", 0, 0);
        if (tl != NULL)
        {
            tex->ptr = tl;
        }*/

        // Setup a scene and place a banner in it
        /*b = new Banner(PVR_LIST_TR_POLY, txr);
        b->setTranslate(Vector(0, 0, 0));
        m_scene->subAdd(b);*/
        m_cursel = 0;
        stage = 9;
        
        gd_cdrom_init();
    }

    virtual ~MyMenu()
    {
    }

    void updateDiscLabel()
    {
        disc_label->setText(updateGD->getTitle());
        char name[20];
        sprintf(name, "[%s]",updateGD->getBinary());
        status_label->setText(name);
        m_options[2]->setText("Start");
        stage = 0;
    }

    virtual void inputEvent(const Event &evt)
    {
        if (evt.type != Event::EvtKeypress)
            return;

        switch (evt.key)
        {
        case Event::KeyUp:
            m_cursel--;
            if (stage == 9 && m_cursel == 2)
            {
                m_cursel--;
            }

            if (m_cursel < 0)
                m_cursel += 4;

            break;
        case Event::KeyDown:
            m_cursel++;
            if (stage == 9 && m_cursel == 2)
            {
                m_cursel++;
            }

            if (m_cursel >= 4)
                m_cursel -= 4;

            break;
        case Event::KeySelect:
            switch (m_cursel)
            {
            case 0:
                status_label->setText("Loading Prev Disc!");
                updateGD->prev();
                updateDiscLabel();
                break;
            case 1:
                status_label->setText("Loading Next Disc!");
                updateGD->next();
                updateDiscLabel();
                break;
            case 2:
                switch (stage)
                {
                case 9:
                    break;
                case 0:
                    if (!updateGD->readIPtoMem())
                    {
                        m_options[2]->setText("Read IP to memory");
                    }
                    else
                    {
                        m_options[2]->setText("Error!");
                    }
                    fflush(stdout);
                    stage++;
                    break;
                case 1:
                    if (!updateGD->readBinaryToMem())
                    {
                        m_options[2]->setText("Read Binary to memory");
                    }
                    else
                    {
                        m_options[2]->setText("Error!");
                    }
                    fflush(stdout);
                    stage++;
                    break;
                case 2:
                    m_options[2]->setText("Execute!");
                    status_label->setText("Executing!");
                    stage++;
                    break;
                case 3:
                    /* Should Work but Broken */
                    /*updateGD->run();*/
                    //Slower but working at the moment
                    updateGD->run_alt();
                    break;
                }
                break;
            case 3:
                if (stage == 3)
                {
                    updateGD->run();
                }
                status_label->setText("Exiting!");
                startExit();
                break;
            }
        default:
            break;
        }

        for (int i = 0; i < 4; i++)
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

        m = new ExpXYMover(0, 1.6, 0, 400);
        m->triggerAdd(new Death());
        m_options[3]->animAdd(m);
        GenericMenu::startExit();
    }

    Color m_white, m_gray, m_red;
    RefPtr<Label> m_options[6];
    RefPtr<Label> title_label, disc_label, status_label;
    RefPtr<Texture> txr;
    RefPtr<Banner> b;
    RefPtr<UpdateGD> updateGD;
    int m_cursel;

  private:
    char stage;
};

int main(int argc, char **argv)
{
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

    // Ok, we're all done! The RefPtrs will take care of mem cleanup.
    arch_menu();
    return 0;
}
