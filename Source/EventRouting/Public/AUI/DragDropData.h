#pragma once

namespace AUI
{
/**
 * A parent class for passing data through a DragDrop event.
 * 
 * To use this, make a derived class that contains whatever data you wish 
 * to pass. Then, in your DragDrop-enabled widget, override getDragDropData() 
 * and return your derived class with your data.
 * When the widget is dropped, the target will receive an instance of this 
 * class, which it can then cast to your derived type and use.
 */
class DragDropData {
    // Note: This is currently always assumed to hold user data. If we ever 
    //       need to support OS drag/drop events, we can add an enum here for 
    //       Image/Text/UserContent/etc that lets the user know which type 
    //       they should cast this to.
};

} // namespace AUI
