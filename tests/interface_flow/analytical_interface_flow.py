''' Analytical solution to compute pressures across interface
    by ensuring continuity of flow across the interface.
    An exploded view is like this:
        +----------+-----------+---------------+
        |  block 1 |  block 3  |    block 2    |
        |    k1    |     k3    |      k2       |
        +----------+-----------+---------------+
        <-- D1  --> <--  D3 --> <----  D2 ----->
        P1        P'1          P'2             P2

'''


k1 = 1 # permeability of block 1
k2 = 1 # permeability of block 2
k3 = 1./10. # permeability of block 3
D1 = 1 # thickness of block 1
D2 = 1 # thickness of block 2
D3 = 0.1 # thickness of block 3
P1 = 1 # pressure on left hand side of block 1
P2 = 0 # pressure on right hand side of block 2

# P_prime_1 = pressure on right hand side of block 1
# P_prime_2 = pressure on left hand side of block 2
P_prime_2 = (k1*k3*D2*D3*P1 + k2*D3*(k1*D3+k3*D1)*P2) / ((k3*D2+k2*D3)*(k1*D3+k3*D1)-k3*k3*D1*D2)
P_prime_1 = (k1*D3*P1 + k3*D1*P_prime_2) / (k1*D3+k3*D1)


print 'P_prime_1 = ', P_prime_1
print 'P_prime_2 = ', P_prime_2
