import unittest
from gestures import ControllerGesture


class ChordOrder(unittest.TestCase):
    def test_either_button_order_starts_one_hold_without_rearming_held_buttons(self):
        device=dict(backend='xinput',slot=0,name='Test pad')
        for first in (0x100,0x2000):
            gate=ControllerGesture(dict(device=device,lb_mask=0x100),
                dict(device=device,lb_mask=0x100,circle_mask=0x2000,hold_seconds=.08),1000)
            gate.process(dict(device,kind='input_device'),1)
            gate.process(dict(device,kind='input',buttons=0),2)
            gate.process(dict(device,kind='input',buttons=first),3)
            gate.process(dict(device,kind='input',buttons=0x2100),4)
            self.assertFalse(gate.fields(83)['armed'])
            self.assertTrue(gate.fields(84)['armed'])
            gate.dispatched();gate.process(dict(device,kind='input',buttons=0x2000),85)
            gate.process(dict(device,kind='input',buttons=0x2100),86)
            self.assertFalse(gate.fields(200)['armed'])
