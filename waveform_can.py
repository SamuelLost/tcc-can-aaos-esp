import matplotlib.pyplot as plt

def plot_can_frame(sof, ids, dlc, data, eof):
    canh_signal = []
    canl_signal = []
    offset = 1
    frame_bits = sof + ids + dlc + data + eof
    labels = ['SOF'] + ['ID'] * 11 + ['DLC'] * 4 + ['D'] * 8 + ['EOF'] * 7
    for i in range(0, len(frame_bits)):
        if frame_bits[i] == 1:
            # canh_signal.append(0.1)
            # canl_signal.append(0.04)
            canh_signal.append(2.5)
            canl_signal.append(2.4)
        else:
            # canh_signal.append(0.7)
            # canl_signal.append(-0.7)
            canh_signal.append(3.5)
            canl_signal.append(1.4)
    for i in range(0,len(frame_bits)):
    #   frame_bits[i] = frame_bits[i]+1
        if frame_bits[i] == 1:
            frame_bits[i] = 4.8
        else:
            frame_bits[i] = 3.8

    tempo_latex = r'$\mathbf{\mu s}$'
    volt_latex = r'$\mathbf{V}$'
    volt_norm = r'$V$'

    plt.figure(figsize=(14, 6))
    plt.step(range(len(frame_bits)), frame_bits, 'go-', label='Bitstream', where='mid')
    plt.step(range(len(canh_signal)), canh_signal, '^-', label='CAN High', where='mid')
    plt.step(range(len(canl_signal)), canl_signal, 'r-v', label='CAN Low', where='mid')
    plt.xlabel(f'Tempo ({tempo_latex})', fontsize=12, fontweight='bold')
    plt.xticks([])
    plt.xticks(range(len(frame_bits)), labels)
    plt.gca().xaxis.set_ticks_position('top')
    # plt.xlabel('Bits', fontsize=12, fontweight='bold')
    # plt.xlabel(labels)
    plt.ylabel(f'Tensão ({volt_latex})', rotation='horizontal', loc='top', labelpad=-30, fontsize=12, verticalalignment='center', fontweight='bold')
    plt.title('Níveis de tensão do CAN', fontsize=14, fontweight='bold', pad=30)

    # for x, label in enumerate(labels):
    #     plt.annotate(label, (x, canh_signal[x]), textcoords="offset points", xytext=(0, 10), ha='center')
    #     plt.annotate(label, (x, canl_signal[x]), textcoords="offset points", xytext=(0, -20), ha='center')

    plt.yticks([0.5, 1.4, 2.45, 3.5], [f'0{volt_norm}', f'1.5{volt_norm}', f'2.5{volt_norm}', f'3.5{volt_norm}'], fontsize=12)
    # Put the labels Domiante and Recessive on right the y-axis
    # plt.yticks([1.4, 2.45, 3.5], ['Dominante', 'Recessivo', 'Dominante'], fontsize=12)
    # plt.yticks([-0.7, (0.1-0.04), 0.7], ['1.5V', '2.5V', '3.5V'])
    plt.grid(True)
    plt.legend(loc='upper left')
    plt.rc('text', usetex=True)
    # plt.savefig('can_waveform4.pdf', dpi=1200, bbox_inches='tight', format='pdf', transparent=True)
    plt.savefig('can_waveform_2000dpi.png', dpi=1600, bbox_inches='tight', format='png', transparent=True)
    plt.show()


val = [0x0, 0x123, 0x4, 0x56, 0x7F]

start = int(val[0])
ids_rcv = int(val[1])
dlc_rcv = chr(int(val[2]))
data_rcv = ord(chr(int(val[3])))
end = int(val[4])


ids = [int(bit) for bit in bin(ids_rcv)[2:].zfill(11)]
# print(ids)
data = [int(bit) for bit in bin(data_rcv)[2:].zfill(8)]
dlc = [int(bit) for bit in bin(len(dlc_rcv))[2:].zfill(4)]
sof = [int(bit) for bit in bin(start)[2:].zfill(1)]
eof = [int(bit) for bit in bin(end)[2:].zfill(7)]


plot_can_frame(sof,ids,dlc,data,eof)
