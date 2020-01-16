.data
_prompt: .asciiz "Enter an integer: "
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra
write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra
main:
  addi $sp, $sp, -84
  sw $ra, 80($sp)
  sw $fp, 76($sp)
  addi $fp, $sp, 76

  lw $t0, -4($fp)
  li $t0, 12
  lw $t1, -8($fp)
  li $t1, 4
  lw $t2, -12($fp)
  li $t2, 5
  lw $t3, -16($fp)
  mul $t3, $t0, $t1
  lw $t4, -20($fp)
  move $t4, $t3
  lw $t5, -24($fp)
  add $t5, $t4, $t2
  lw $t6, -28($fp)
  sub $t6, $t5, $t1
  lw $t7, -32($fp)
  move $t7, $t6
  lw $s0, -36($fp)
  mul $s0, $t7, $t1
  lw $s1, -40($fp)
  add $s1, $t4, $s0
  lw $s2, -44($fp)
  move $s2, $s1
  move $a0, $s2
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4

  lw $s3, -48($fp)
  add $s3, $t4, $t7
  lw $s4, -52($fp)
  mul $s4, $s2, $s3
  lw $s5, -56($fp)
  li $s6, 25
  div $s4, $s6
  mflo $s5
  move $s2, $s5
  lw $s6, -60($fp)
  add $s6, $t7, $s2
  lw $s7, -64($fp)
  div $t4, $s2
  mflo $s7
  sw $s5, -56($fp)
  lw $s5, -68($fp)
  sub $s5, $s6, $s7
  sw $s7, -64($fp)
  lw $s7, -72($fp)
  li $t8, 12
  mul $s7, $t1, $t8
  sw $s6, -60($fp)
  lw $s6, -76($fp)
  add $s6, $s5, $s7
  move $t7, $s6
  move $a0, $s2
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4

  move $a0, $t7
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4

  li $t8, 0
  move $v0, $t8
  lw $ra, 4($fp)
  lw $fp, 0($fp)
  addi $sp, $sp, 84
  jr $ra
  #saveAll:
  sw $t0, -4($fp)
  sw $t1, -8($fp)
  sw $t2, -12($fp)
  sw $t3, -16($fp)
  sw $t4, -20($fp)
  sw $t5, -24($fp)
  sw $t6, -28($fp)
  sw $t7, -32($fp)
  sw $s0, -36($fp)
  sw $s1, -40($fp)
  sw $s2, -44($fp)
  sw $s3, -48($fp)
  sw $s4, -52($fp)
  sw $s5, -68($fp)
  sw $s7, -72($fp)
  sw $s6, -76($fp)
  #endsave
