/**
 *  MIT License
 *  Copyright (c) Pavel Hromada
 *  See accompanying LICENSE file
 */

#ifndef KEYBOARDWINDOWPOSITIONER_H
#define KEYBOARDWINDOWPOSITIONER_H

#include <memory>
#include <QPointer>
#include <QPropertyAnimation>
#include "commonpositioner.h"

class QQuickWindow;
class QQuickItem;
class QScreen;

class KeyboardWindowPositioner : public CommonPositioner
{
public:
    /**
     * @brief Construct a new Keyboard Window Positioner object
     *
     * @param _screen_idx Specify the screen on which keyboard appear in
     * multiscreen setup
     */
    explicit KeyboardWindowPositioner(int screen_idx = -1);
    ~KeyboardWindowPositioner() override;
    void setKeyboardObject( QObject* keyboardObject ) override;
    void enableAnimation( bool enabled ) override;
    void updateFocusItem( QQuickItem* focusItem ) override;
    void show() override;
    void hide() override;
    bool isAnimating() const override;

private:
    void initKeyboardWindow();
    void hide( bool suppressAnimation );
    void setupKeyboardWindowMask();
    void observeWindowOfFocusedItem();
    void updateMask();
    void onAnimationFinished();
    void onScreenChanged( QScreen* screen );
    void onWindowVisibleChanged( bool visible );

    int _screen_idx;
    bool                                _shown = false;
    QPointer<QQuickWindow>              _keyboardWindow;
    QPointer<QQuickItem>                _keyboard;
    std::unique_ptr<QPropertyAnimation> _animation;
    QPointer<QQuickItem>                _focusItem;
};

#endif // KEYBOARDWINDOWPOSITIONER_H
