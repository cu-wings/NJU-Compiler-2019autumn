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
  addi $sp, $sp, -48
  sw $ra, 44($sp)
  sw $fp, 40($sp)
  addi $fp, $sp, 40

  li $t0, 0
  li $t1, 4
  lw $t2, -12($fp)
  mul $t2, $t0, $t1
  lw $t0, -8($fp)
  lw $t1, -16($fp)
  add $t1, $t0, $t2
  li $v1, 1
  sw $v1, 0($t1)
  li $t3, 1
  li $t4, 4
  lw $t5, -20($fp)
  mul $t5, $t3, $t4
  lw $t3, -24($fp)
  add $t3, $t0, $t5
  li $v1, 2
  sw $v1, 0($t3)
  li $t4, 0
  li $t6, 4
  lw $t7, -28($fp)
  mul $t7, $t4, $t6
  lw $t4, -32($fp)
  add $t4, $t0, $t7
  move $a0, $t4
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4

  li $t6, 1
  li $s0, 4
  lw $s1, -36($fp)
  mul $s1, $t6, $s0
  lw $t6, -40($fp)
  add $t6, $t0, $s1
  move $a0, $t6
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4

  li $s0, 0
  move $v0, $s0
  lw $ra, 4($fp)
  lw $fp, 0($fp)
  addi $sp, $sp, 48
  jr $ra
