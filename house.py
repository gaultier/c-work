import re
import sys
import time
from operator import itemgetter


def distance(coord1, coord2):
    return (coord2[0] - coord1[0])**2 + (coord2[1] - coord1[1])**2


def brute_force(coords, split):
    min_distance = float('inf')
    for i in range(len(coords)):
        high = min(i + 8, len(coords)) if split else len(coords)
        for j in range(i + 1, high):
            current_distance = distance(coords[i], coords[j])
            if current_distance < min_distance:
                min_distance = current_distance
                closest_coords = (coords[i], coords[j])
    return closest_coords, min_distance


def closest_pair(coords):

    if len(coords) <= 3:
        return brute_force(coords, False)

    mid_point = len(coords) // 2
    left_closest_pair = closest_pair(coords[:mid_point])
    right_closest_pair = closest_pair(coords[mid_point:])
    min_distance = min(left_closest_pair, right_closest_pair, key = itemgetter(1))

    split_pairs = [coord for coord in coords
                   if abs(coord[0] - coords[mid_point][0])**2 < min_distance[1]]
    split_pairs = sorted(split_pairs, key = itemgetter(1))

    if len(split_pairs) > 1:
        closest_strip_pair = brute_force(split_pairs, True)
    else:
        closest_strip_pair = (float('inf'), float('inf'))

    final_pair = min(closest_strip_pair, min_distance, key = itemgetter(1))

    return final_pair


def main(input_text):

    # Read in and organize points
    with open(input_text, 'r') as infile:
        N = infile.readline()
        rgx = re.compile(r'[-\w.]+')
        lines = [rgx.findall(line) for line in infile.readlines()]

    # Coordinate pairs are sorted by X coordinate
    coords = sorted([(float(x[0]), float(x[1])) for x in lines])

    print('N = ' + str(N))

    # Perform and time brute force (for less than 100k coordinates)
    if int(N) < 100000:
        start = time.clock()
        brute_force_coords = brute_force(coords, False)
        brute_force_time = round(time.clock() - start, 2)
        print('Brute force:        {}, {}  {} seconds'.format(str(brute_force_coords[0][0]),
                                                              str(brute_force_coords[0][1]),
                                                              str(brute_force_time)))
    # Perform and time divide and conquer
    start = time.clock()
    closest_pair_coords = closest_pair(coords)
    closest_pair_time = round(time.clock() - start, 2)
    print('Divide and conquer: {}, {} - {}  {} seconds'.format(str(closest_pair_coords[0][0]),
                                                          str(closest_pair_coords[0][1]),
                                                          str(distance(closest_pair_coords[0][0], closest_pair_coords[0][1])),
                                                          str(closest_pair_time)))


if __name__ == '__main__':
    main(sys.argv[1])
