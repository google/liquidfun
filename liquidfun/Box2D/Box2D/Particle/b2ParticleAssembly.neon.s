@
@ Copyright (c) 2014 Google, Inc.
@
@ This software is provided 'as-is', without any express or implied
@ warranty.  In no event will the authors be held liable for any damages
@ arising from the use of this software.
@ Permission is granted to anyone to use this software for any purpose,
@ including commercial applications, and to alter it and redistribute it
@ freely, subject to the following restrictions:
@ 1. The origin of this software must not be misrepresented; you must not
@ claim that you wrote the original software. If you use this software
@ in a product, an acknowledgment in the product documentation would be
@ appreciated but is not required.
@ 2. Altered source versions must be plainly marked as such, and must not be
@ misrepresented as being the original software.
@ 3. This notice may not be removed or altered from any source distribution.
@
        .text
        .syntax   unified

        .balign   4
        .global   CalculateTags_Simd
        .thumb_func

CalculateTags_Simd:
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        @
        @ int CalculateTags_Simd(const b2Vec2* positions,
        @                        int count,
        @                        const float& inverseDiameter,
        @                        uint32* outTags)
        @
        @  r0: *positions
        @  r1: count
        @  r2: &inverseDiameter
        @  r3: *outTags
        @
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        @  q0 == x
        @  q1 == y
        @  q2 ==
        @  q3 ==
        @  q4 ==
        @  q5 ==
        @  q6 ==
        @  q7 ==
        @  q8 ==
        @  q9 ==
        @ q10 ==
        @ q11 ==
        @ q12 == inverseDiameter
        @ q13 == xScale
        @ q14 == xOffset
        @ q15 == yOffset

        @ Load constants. Literals are > 32, so must load as integers first.
        vld1.f32          {d24[],d25[]}, [r2] @ q12 = inverseDiameter
        vmov.i32          q13, #0x100   @ q13 = xScale = 1 << 8
        vmov.i32          q14, #0x80000 @ q14 = xOffset = (1 << 8) * (1 << 11)
                                        @               = (1 << 19) = 524288
        vmov.i32          q15, #0x800   @ q15 = xScale = 1 << 11 = 2048
        vcvt.f32.u32      q13, q13      @ convert to float
        vcvt.f32.u32      q14, q14
        vcvt.f32.u32      q15, q15

        @ Calculate tags four at a time, from positions.
.L_CalculateTags_MainLoop:
        @ We consume 32-bytes per iteration, so prefetch 4 iterations ahead.
        @ TODO: experiment with different prefetch lengths on different
        @ architectures.
        pld               [r0, #128]    @ Prefetch position data

        @ {q0, q1} == xPosition and yPosition
        @ Four values in each. q0 = (x0, x1, x2, x3)
        vld2.f32          {q0, q1}, [r0]! @ Read in positions; increment ptr

        @ Calculate tags four at a time.
        vmul.f32          q0, q0, q12   @ q0 = x = xPosition * inverseDiameter
        vmul.f32          q1, q1, q12   @ q1 = y = yPosition * inverseDiameter
        vmul.f32          q0, q0, q13   @ q0 = x * xScale
        vadd.f32          q1, q1, q15   @ q1 = y + yOffset
        vadd.f32          q0, q0, q14   @ q0 = x * xScale + xOffset
        vcvt.u32.f32      q1, q1        @ q1 = (uint32)(y + yOffset)
        vcvt.u32.f32      q0, q0        @ q0 = (uint32)(x * xScale + xOffset)
        vsli.u32          q0, q1, #20   @ q0 = tag
                                        @    = ((uint32)(y + yOffset) <<yShift)
                                        @    + (uint32)(xScale * x + xOffset)

        @ Decrement loop counter; sets the 'gt' flag used in 'bgt' below.
        @ Pipelining is best if there are instructions between the 'subs' and
        @ 'bgt' instructions, since it takes a few cycles for the result of
        @ 'subs' to propegate to the flags register.
        subs              r1, r1, #4

        @ Write out, ignoring index.
        pld               [r3, #64]     @ Prefetch output tag array
        vst1.f32          {q0}, [r3]!   @ write out tags; increment ptr

        bgt               .L_CalculateTags_MainLoop

.L_CalculateTags_Return:
        bx                lr
