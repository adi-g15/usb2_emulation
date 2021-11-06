#pragma once

enum Device_Type { LowSpeed, FullSpeed };

enum D_Lines_State {
    DIFF_1, // Differential 1 (H,L) (D+,D- respectively)
    DIFF_0, // Differential 0 (L,H)
    SE0,    // Single-Ended 0 (L,L)
    SE1,    // Single-Ended 1 (H,H)
};
